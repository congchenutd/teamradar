#include "Connection.h"
#include "PeerManager.h"
#include "Setting.h"
#include <QHostAddress>
#include <QFile>
#include <QFileInfo>

Connection::Connection(QObject* parent) : QTcpSocket(parent)
{
	state = WaitingForGreeting;
	dataType = Undefined;
	numBytes = -1;
	timerId = 0;
	isGreetingSent = false;
	pingTimer.setInterval(PingInterval);
	userName = tr("Unknown");

	connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	connect(this, SIGNAL(connected()), this, SLOT(sendGreeting()));
	connect(this, SIGNAL(disconnected()), &pingTimer, SLOT(stop()));
	connect(&pingTimer, SIGNAL(timeout()), this, SLOT(sendPing()));

	connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

void Connection::onReadyRead()
{
	if(state == WaitingForGreeting)
	{
		if(!readHeader())   // read header and guess data type
			return;
		if(dataType != Greeting)
		{
			abort();
			return;
		}
		state = ReadingGreeting;
	}

	if(state == ReadingGreeting)
	{
		if(!hasEnoughData())        // get data length and make sure there is data
			return;

		buffer = read(numBytes);    // read greeting
		if(buffer.size() != numBytes)
		{
			abort();
			return;
		}
		qDebug() << buffer;

		if(buffer == "WRONG_USER")
		{
			abort();
			emit connectionFailed(buffer);
			return;
		}

		dataType = Undefined;
		numBytes = 0;
		buffer.clear();

		if(!isValid())   // why do we need this??
		{
			abort();
			return;
		}

		pingTimer.start();     // start heart beat
		pongTime.start();      // wait for peer's pong
		state = ReadyForUse;
		emit readyForUse();
	}

	do
	{
		if(dataType == Undefined && !readHeader())   // read header
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
	if(timerId)
	{
		killTimer(timerId);
		timerId = 0;
	}

	// read header data
	if(readDataIntoBuffer() <= 0)
	{
		timerId = startTimer(TransferTimeout);
		return false;
	}

	dataType = guessDataType(buffer);  // guess payload type from header
	if(dataType == Undefined)
		return false;

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
		if(buffer.endsWith('#'))
			break;
	}

	return buffer.size() - numBytesBeforeRead;
}

// read length info
int Connection::getDataLength()
{
	// check if there are bytes, then read, and finally check ending
	if (bytesAvailable() <= 0 || readDataIntoBuffer() <= 0 || !buffer.endsWith('#'))
		return 0;

	buffer.chop(1);    // chop last char, the separator
	int number = buffer.toInt();
	buffer.clear();
	return number;
}

void Connection::sendPing()
{
	if(pongTime.elapsed() > PongTimeout)
	{
		abort();   // peer dead
		return;
	}

	write("PING#" + QByteArray::number(1) + '#' + "P");
}

void Connection::sendGreeting()
{
	QByteArray greeting = userName.toUtf8();
	QByteArray data = "GREETING#" + QByteArray::number(greeting.size()) + '#' + greeting;
	if(write(data) == data.size())
		isGreetingSent = true;
}

// read data length and wait for it
bool Connection::hasEnoughData()
{
	// new timerid
	if(timerId)
	{
		QObject::killTimer(timerId);
		timerId = 0;
	}

	// get length
	if(numBytes <= 0)
		numBytes = getDataLength();

	// wait for data
	if(bytesAvailable() < numBytes || numBytes <= 0)
	{
		timerId = startTimer(TransferTimeout);
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

	switch(dataType)
	{
	case Ping:
		write("PONG#" + QByteArray::number(1) + '#' + "P");
		break;
	case Pong:
		pongTime.restart();
		break;
	case Event:
		emit newMessage(QString::fromUtf8(buffer));
		break;
	case PhotoResponse:
		emit photoResponse(buffer);
		break;
	case UserListResponse:
		emit userList(buffer);
		break;
	default:
		break;
	}

	dataType = Undefined;
	numBytes = 0;
	buffer.clear();
}

Connection::DataType Connection::guessDataType(const QByteArray& header)
{
	if(header.startsWith("GREETING"))
		return Greeting;
	if(header.startsWith("PING"))
		return Ping;
	if(header.startsWith("PONG"))
		return Pong;
	if(header.startsWith("EVENT"))
		return Event;
	if(header.startsWith("REGISTER_RESPONSE"))   // useless, server does not respond
		return RegisterResponse;
	if(header.startsWith("PHOTO_RESPONSE"))
		return PhotoResponse;
	if(header.startsWith("USERLIST_RESPONSE"))
		return UserListResponse;
	return Undefined;
}

Connection* Connection::getInstance(QObject* parent)
{
	if(instance == 0)
		instance = new Connection(parent);
	return instance;
}

Connection* Connection::instance = 0;

void Connection::timerEvent(QTimerEvent* timerEvent)
{
	if(timerEvent->timerId() == timerId) {
		abort();
		killTimer(timerId);
		timerId = 0;
	}
}

void Connection::onDisconnected()
{
	Setting* setting = MySetting<Setting>::getInstance();
	connectToHost(setting->getServerAddress(), setting->getServerPort());
}