#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QTimer>
#include <QTime>
#include <QStringList>
#include "TeamRadarEvent.h"

// Parses the message header & body from Connection
// Format of packet: header#size#body
// Format of body:
//		GREETING: [OK, CONNECTED]/[WRONG_USER]
//		PHOTO_RESPONSE: [filename#binary photo data]/[empty]
//		USERLIST_RESPONSE: username1#username2#...
//		EVENT: user#event#[parameters]#time
//			Format of parameters: parameter1#parameter2#...
//		COLOR_RESPONSE: [username#color]/[empty]
//		TIMESPAN_RESPONSE: start#end

class Receiver : public QObject
{
	Q_OBJECT

public:
	typedef enum {
		Undefined,
		Greeting,
		Event,
		PhotoResponse,
		UserListResponse,
		ColorResponse,
		EventsResponse,
		Chat,
		TimeSpanResponse
	} DataType;

public:
	static Receiver* getInstance();

	Receiver();
	DataType guessDataType(const QByteArray& header);
	void processData(Receiver::DataType dataType, const QByteArray& buffer);

signals:
	void newEvent(const TeamRadarEvent& event);
	void userList(const QList<QByteArray>& list);
	void photoResponse(const QString& fileName,   const QByteArray& photoData);
	void colorResponse(const QString& targetUser, const QByteArray& color);
	void eventsResponse(const TeamRadarEvent& event);
	void chatMessage(const QString& peerName, const QString& content);
	void timespan(const QDateTime& start, const QDateTime& end);

private:
	void parseGreeting(const QByteArray& buffer);
	void parseEvent   (const QByteArray& buffer);
	void parseUserList(const QByteArray& buffer);
	void parsePhoto   (const QByteArray& buffer);
	void parseColor   (const QByteArray& buffer);
	void parseEvents  (const QByteArray& buffer);
	void parseChat    (const QByteArray& buffer);
	void parseTimeSpan(const QByteArray& buffer);

private:
	static Receiver* instance;
	int eventCount;
	int eventsReceived;
};


// A TCP socket connected to the server
// A singleton: there is one connection from the client to the server
class Connection : public QTcpSocket
{
	Q_OBJECT

public:
	static Connection* getInstance(QObject* parent = 0);

	Connection(QObject* parent = 0);
	QString getUserName()   const { return userName; }
	bool    isReadyForUse() const { return ready;    }
	void setUserName(const QString& name) { userName = name; }
	void setReadyForUse();

	void send(const QByteArray& header, const QByteArray& body = QByteArray("P"));
	void send(const QByteArray& header, const QList<QByteArray>& bodies);

protected:
	void timerEvent(QTimerEvent* timerEvent);

signals:
	void connectionStatusChanged(bool);

private slots:
	void onReadyRead();
	void sendGreeting();
	void onDisconnected();

private:
	bool readHeader();
	int  readDataIntoBuffer(int maxSize = MaxBufferSize);
	int  getDataLength();
	bool hasEnoughData();
	void processData();

public:
	static const int MaxBufferSize   = 1024 * 1024;
	static const int TransferTimeout = 30 * 1000;
	static const char Delimiter1 = '#';
	static const char Delimiter2 = ';';
	static const char Delimiter3 = ',';

private:
	static Connection* instance;

	Receiver::DataType dataType;
	bool       ready;
	QByteArray buffer;
	int        numBytes;
	int        transferTimerID;
	QString    userName;
	Receiver*  receiver;
};

// Format of packet: header#size#body
// Does not need to send my user name, because the server knows who I am
// Format of body:
//		REQUEST_USERLIST: [empty], server knows the user name
//		REQUEST_PHOTO: target user name
//		REGISTER_PHOTO: file format#binary photo data
//		EVENT: event type#parameters
//			Format of parameters: parameter1#parameter2#...
//		REGISTER_COLOR: color
//		REQUEST_COLOR: target user name
//		REQUEST_EVENTS: user list#time span#event types
//			user list: name1;name2;...
//			event types: type1;type2;...
//			time span: start time;end time
//		CHAT: recipients#content
//			recipients = name1;name2;...
//		REQUEST_TIMESPAN: [empty]

class Sender : public QObject
{
public:
	static Sender* getInstance();
	Sender();
	void sendEvent(const QString& event, const QString& parameters);
	void sendUserListRequest();
	void sendPhotoRegistration(const QByteArray& format, const QByteArray& photoData);
	void sendColorRegistration(const QColor& color);
	void sendPhotoRequest(const QString& targetUser);
	void sendColorRequest(const QString& targetUser);
	void sendEventRequest(const QStringList& users, const QDateTime& startTime, 
						  const QDateTime& endTime, const QStringList& eventTypes);
	void sendChat(const QStringList& recipients, const QString& content);
	void sendTimeSpanRequest();

private:
	static Sender* instance;
	Connection* connection;
};

#endif // CONNECTION_H
