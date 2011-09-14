#include "Connection.h"
#include "PeerManager.h"
#include "Setting.h"
#include <QHostAddress>
#include <QFile>
#include <QFileInfo>
#include <QTimerEvent>

Connection::Connection(QObject* parent) : QTcpSocket(parent)
{
	ready = false;
	dataType = Receiver::Undefined;
	numBytes = -1;
	transferTimerID = 0;
	userName = Setting::getInstance()->getUserName();
	receiver = Receiver::getInstance();

	connect(this, SIGNAL(readyRead()),    this, SLOT(onReadyRead()));
	connect(this, SIGNAL(connected()),    this, SLOT(sendGreeting()));
	connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

void Connection::onReadyRead()
{
	do {
		if(dataType == Receiver::Undefined && !readHeader())   // read header
			return;
		if(!hasEnoughData())                         // read data
			return;
		processData();                               // process
	} while(bytesAvailable() > 0);                   // more bytes on the stream
}

// get the data type and length
bool Connection::readHeader()
{
	// new timerid
	if(transferTimerID)
	{
		killTimer(transferTimerID);
		transferTimerID = 0;
	}

	// read header data
	if(readDataIntoBuffer() <= 0)
	{
		transferTimerID = startTimer(TransferTimeout);
		return false;
	}

	dataType = receiver->guessDataType(buffer);  // guess payload type from header
	if(dataType == Receiver::Undefined)
	{
		buffer.clear();
		return false;
	}

	buffer.clear();
	numBytes = getDataLength();
	return true;
}

// read all available data to buffer until (including) the separator
int Connection::readDataIntoBuffer(int maxSize)
{
	if (maxSize > MaxBufferSize)
		return 0;

	int numBytesBeforeRead = buffer.size();
	if(numBytesBeforeRead == MaxBufferSize)  // buffer full
	{
		abort();
		return 0;
	}

	// read until separator
	while(bytesAvailable() > 0 && buffer.size() < maxSize)
	{
		buffer.append(read(1));
		if(buffer.endsWith(Delimiter1))
			break;
	}

	return buffer.size() - numBytesBeforeRead;
}

// read length info
int Connection::getDataLength()
{
	// check if there are bytes, then read, and finally check ending
	if (bytesAvailable() <= 0 || readDataIntoBuffer() <= 0 || !buffer.endsWith(Delimiter1))
		return 0;

	buffer.chop(1);    // chop last char, the separator
	int number = buffer.toInt();
	buffer.clear();
	return number;
}

void Connection::sendGreeting() {
	send("GREETING", userName.toUtf8());
}

// read data length and wait for it
bool Connection::hasEnoughData()
{
	// new timerid
	if(transferTimerID)
	{
		QObject::killTimer(transferTimerID);
		transferTimerID = 0;
	}

	// get length
	if(numBytes <= 0)
		numBytes = getDataLength();

	// wait for data
	if(bytesAvailable() < numBytes/* || numBytes <= 0*/)
	{
		transferTimerID = startTimer(TransferTimeout);
		return false;
	}

	return true;
}

void Connection::processData()
{
	buffer = read(numBytes);
	if(buffer.size() != numBytes)
	{
		abort();
		return;
	}

	receiver->processData(dataType, buffer);

	dataType = Receiver::Undefined;
	numBytes = 0;
	buffer.clear();
}

Connection* Connection::getInstance(QObject* parent)
{
	if(instance == 0)
		instance = new Connection(parent);
	return instance;
}

Connection* Connection::instance = 0;

void Connection::timerEvent(QTimerEvent* timerEvent) {
	if(timerEvent->timerId() == transferTimerID) {
		abort();
		killTimer(transferTimerID);
		transferTimerID = 0;
	}
}

// auto reconnect
void Connection::onDisconnected()
{
	emit connectionStatusChanged(false);

	 // reconnect
	Setting* setting = Setting::getInstance();
	connectToHost(setting->getServerAddress(), setting->getServerPort());
}

void Connection::send(const QByteArray& header, const QByteArray& body)
{
//	QByteArray b = body.isEmpty() ? "P" : body;
	QByteArray message(header);
	if(!header.endsWith(Delimiter1))
		message.append(Delimiter1);
	message.append(QByteArray::number(body.length()) + Delimiter1 + body);
	write(message);
}

void Connection::send(const QByteArray& header, const QList<QByteArray>& bodies)
{
	QByteArray joined;
	foreach(QByteArray body, bodies)
		joined.append(body + Delimiter1);
	joined.chop(1);  // remove last '#'
	send(header, joined);
}

void Connection::setReadyForUse()
{
	ready = true;
	emit connectionStatusChanged(true);
}

//////////////////////////////////////////////////////////////////////////
Receiver* Receiver::instance = 0;

Receiver* Receiver::getInstance()
{
	if(instance == 0)
		instance = new Receiver;
	return instance;
}

Receiver::Receiver()
{
	dataTypes.insert("GREETING",          Greeting);
	dataTypes.insert("EVENT_RESPONSE",    EventsResponse);
	dataTypes.insert("EVENT",             Event);
	dataTypes.insert("PHOTO_RESPONSE",    PhotoResponse);
	dataTypes.insert("USERLIST_RESPONSE", UserListResponse);
	dataTypes.insert("COLOR_RESPONSE",    ColorResponse);
	dataTypes.insert("CHAT",              Chat);
	dataTypes.insert("TIMESPAN_RESPONSE", TimeSpanResponse);
	dataTypes.insert("PROJECTS_RESPONSE", ProjectsResponse);

	parsers.insert(Greeting,         &Receiver::parseGreeting);
	parsers.insert(EventsResponse,   &Receiver::parseEvents);
	parsers.insert(Event,            &Receiver::parseEvent);
	parsers.insert(UserListResponse, &Receiver::parseUserList);
	parsers.insert(PhotoResponse,    &Receiver::parsePhoto);
	parsers.insert(ColorResponse,    &Receiver::parseColor);
	parsers.insert(Chat,             &Receiver::parseChat);
	parsers.insert(TimeSpanResponse, &Receiver::parseTimeSpan);
	parsers.insert(ProjectsResponse, &Receiver::parseProjects);
}

Receiver::DataType Receiver::guessDataType(const QByteArray& h)
{
	QByteArray header = h;
	if(header.endsWith('#'))
		header.chop(1);
	return dataTypes.contains(header) ? dataTypes[header] : Undefined;
}

void Receiver::processData(Receiver::DataType dataType, const QByteArray& buffer) {
	if(dataType != Undefined)
		(this->*parsers[dataType])(buffer);
}

void Receiver::parseGreeting(const QByteArray& buffer)
{
	if(buffer == "WRONG_USER")
		abort();
	else
		Connection::getInstance()->setReadyForUse();
}

void Receiver::parseEvent(const QByteArray& buffer)
{
	QList<QByteArray> sections = buffer.split(Connection::Delimiter1);
	if(sections.size() == 4)
		emit newEvent(TeamRadarEvent(sections[0], sections[1], sections[2], sections[3]));
}

void Receiver::parseUserList(const QByteArray& buffer) {
	emit userList(buffer.split(Connection::Delimiter1));
}

void Receiver::parsePhoto(const QByteArray& buffer)
{
	int seperator = buffer.indexOf(Connection::Delimiter1);
	if(seperator != -1)
	{
		QString fileName = buffer.left(seperator);
		QByteArray photoData = buffer.right(buffer.length() - seperator - 1);
		emit photoResponse(fileName, photoData);
	}
}

void Receiver::parseColor(const QByteArray& buffer)
{
	int seperator = buffer.indexOf(Connection::Delimiter1);
	if(seperator != -1)
	{
		QString targetUser = buffer.left(seperator);
		QByteArray color = buffer.right(buffer.length() - seperator - 1);
		emit colorResponse(targetUser, color);
	}
}

void Receiver::parseEvents(const QByteArray& buffer)
{
	QList<QByteArray> sections = buffer.split(Connection::Delimiter1);
	if(sections.size() == 4)
		emit eventsResponse(TeamRadarEvent(sections[0], sections[1], sections[2], sections[3]));
}

void Receiver::parseChat(const QByteArray& buffer)
{
	int seperator = buffer.indexOf(Connection::Delimiter1);
	if(seperator != -1)
	{
		QString peerName = buffer.left(seperator);
		QString content = buffer.right(buffer.length() - seperator - 1);
		emit chatMessage(peerName, content);
	}
}

void Receiver::parseTimeSpan(const QByteArray& buffer)
{
	QList<QByteArray> sections = buffer.split(Connection::Delimiter1);
	if(sections.size() == 2)
		emit timespan(QDateTime::fromString(sections[0], Setting::dateTimeFormat),
					  QDateTime::fromString(sections[1], Setting::dateTimeFormat));
}

void Receiver::parseProjects(const QByteArray& buffer)
{
	if(!buffer.isEmpty())
		emit projectsResponse(QString(buffer).split(Connection::Delimiter1));
	else
		emit projectsResponse(QStringList());
}

//////////////////////////////////////////////////////////////////////////
Sender* Sender::getInstance()
{
	if(instance == 0)
		instance = new Sender;
	return instance;
}

Sender::Sender() {
	connection = Connection::getInstance();
}

Sender* Sender::instance = 0;

void Sender::sendEvent(const QString& event, const QString& parameters) {
	if(connection->isReadyForUse())
		connection->send("EVENT", QList<QByteArray>() << event.toUtf8() << parameters.toUtf8());
}

void Sender::sendUserListRequest() {
	if(connection->isReadyForUse())
		connection->send("REQUEST_USERLIST");
}

void Sender::sendPhotoRequest(const QString& targetUser) {
	if(connection->isReadyForUse())
		connection->send("REQUEST_PHOTO", targetUser.toUtf8());
}

void Sender::sendPhotoRegistration(const QByteArray& format, const QByteArray& photoData) {
	if(connection->isReadyForUse())
		connection->send("REGISTER_PHOTO", QList<QByteArray>() << format << photoData);
}

void Sender::sendColorRegistration(const QColor& color) {
	if(connection->isReadyForUse())
		connection->send("REGISTER_COLOR", color.name().toUtf8());
}

void Sender::sendColorRequest(const QString& targetUser) {
	if(connection->isReadyForUse())
		connection->send("REQUEST_COLOR", targetUser.toUtf8());
}

void Sender::sendEventRequest(const QStringList& users, const QStringList& eventTypes,
							  const QDateTime& startTime, const QDateTime& endTime,
							  const QStringList& phases, int fuzziness)
{
	if(connection->isReadyForUse())
		connection->send("REQUEST_EVENTS",
			QList<QByteArray>() << users.join(QString(Connection::Delimiter2)).toUtf8()
								<< eventTypes.join(QString(Connection::Delimiter2)).toUtf8()
								<< startTime.toString(Setting::dateTimeFormat).toUtf8() + Connection::Delimiter2 +
								   endTime  .toString(Setting::dateTimeFormat).toUtf8()
								<< phases.join(QString(Connection::Delimiter2)).toUtf8()
								<< QByteArray::number(fuzziness)
		);
}

void Sender::sendChat(const QStringList& recipients, const QString& content) {
	if(connection->isReadyForUse())
		connection->send("CHAT", QList<QByteArray>() << recipients.join(QString(Connection::Delimiter2)).toUtf8()
													 << content.toUtf8());
}

void Sender::sendTimeSpanRequest() {
	if(connection->isReadyForUse())
		connection->send("REQUEST_TIMESPAN");
}

void Sender::sendProjectsRequest() {
	if(connection->isReadyForUse())
		connection->send("REQUEST_PROJECTS");
}

void Sender::sendJoinProject(const QString& projectName) {
	if(connection->isReadyForUse())
		connection->send("JOIN_PROJECT", projectName.toUtf8());
}
