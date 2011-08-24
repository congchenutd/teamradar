#include "Connection.h"
#include "PeerManager.h"
#include "Setting.h"
#include <QHostAddress>
#include <QFile>
#include <QFileInfo>

Connection::Connection(QObject* parent) : QTcpSocket(parent)
{
	ready = false;
	dataType = Receiver::Undefined;
	numBytes = -1;
	transferTimerID = 0;
	userName = tr("Unknown");
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

// read all available data to buffer
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
	if(bytesAvailable() < numBytes || numBytes <= 0)
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
	Setting* setting = MySetting<Setting>::getInstance();
	connectToHost(setting->getServerAddress(), setting->getServerPort());
}

void Connection::send(const QByteArray& header, const QByteArray& body)
{
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
	eventCount = eventsReceived = 0;
}

Receiver::DataType Receiver::guessDataType(const QByteArray& header)
{
	if(header.startsWith("GREETING"))
		return Greeting;
	if(header.startsWith("EVENT_RESPONSE"))  // startsWith is not greedy!!
		return EventsResponse;               // put long prefix first
	if(header.startsWith("EVENT"))
		return Event;
	if(header.startsWith("PHOTO_RESPONSE"))
		return PhotoResponse;
	if(header.startsWith("USERLIST_RESPONSE"))
		return UserListResponse;
	if(header.startsWith("COLOR_RESPONSE"))
		return ColorResponse;
	return Undefined;
}

void Receiver::processData(Receiver::DataType dataType, const QByteArray& buffer)
{
	switch(dataType)
	{
	case Greeting:
		receiveGreeting(buffer);
		break;
	case Event:
		receiveEvent(buffer);
		break;
	case PhotoResponse:
		receivePhoto(buffer);
		break;
	case UserListResponse:
		receiveUserList(buffer);
		break;
	case ColorResponse:
		receiveColor(buffer);
		break;
	case EventsResponse:
		receiveEvents(buffer);
		break;
	default:
		break;
	}
}

void Receiver::receiveGreeting(const QByteArray& buffer)
{
	if(buffer == "WRONG_USER")
		abort();
	else
		Connection::getInstance()->setReadyForUse();
}

void Receiver::receiveEvent(const QByteArray& buffer)
{
	QList<QByteArray> sections = buffer.split(Connection::Delimiter1);
	if(sections.size() == 4)
		emit newEvent(TeamRadarEvent(sections[0], sections[1], sections[2], sections[3]));
}

void Receiver::receiveUserList(const QByteArray& buffer) {
	emit userList(buffer.split(Connection::Delimiter1));
}

void Receiver::receivePhoto(const QByteArray& buffer)
{
	int seperator = buffer.indexOf(Connection::Delimiter1);
	if(seperator != -1)
	{
		QString fileName = buffer.left(seperator);
		QByteArray photoData = buffer.right(buffer.length() - seperator - 1);
		emit photoResponse(fileName, photoData);
	}
}

void Receiver::receiveColor(const QByteArray& buffer)
{
	int seperator = buffer.indexOf(Connection::Delimiter1);
	if(seperator != -1)
	{
		QString targetUser = buffer.left(seperator);
		QByteArray color = buffer.right(buffer.length() - seperator - 1);
		emit colorResponse(targetUser, color);
	}
}

void Receiver::receiveEvents(const QByteArray& buffer)
{
	QList<QByteArray> sections = buffer.split(Connection::Delimiter1);
	if(sections.size() != 4)
		return;
	emit eventsResponse(TeamRadarEvent(sections[0], sections[1], sections[2], sections[3]));
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

void Sender::sendEventRequest(const QStringList& users, const QDateTime& startTime, 
							  const QDateTime& endTime, const QStringList& eventTypes)
{
	if(connection->isReadyForUse())
		connection->send("REQUEST_EVENTS", 
			QList<QByteArray>() << users.join(";").toUtf8()
								<< startTime.toString().toUtf8() + "-" + endTime.toString().toUtf8()
								<< eventTypes.join(";").toUtf8());
}