#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QTimer>
#include <QTime>
#include <QStringList>

// Parse the message header & body from Connection
// Accepts 4 types of header: GREETING, PHOTO_RESPONSE, USERLIST_RESPONSE, EVENT
// Format of packet: header#size#body
// Format of body:
//		GREETING: [OK, CONNECTED]/[WRONG_USER]
//		PHOTO_RESPONSE: [filename#binary photo data]/[empty]
//		USERLIST_RESPONSE: username1#username2#...
//		EVENT: event#parameters
//			Format of parameters: parameter1#parameter2#...
class Receiver : public QObject
{
	Q_OBJECT

public:
	typedef enum {
		Undefined,
		Greeting,
		PhotoResponse,
		UserListResponse,
		Event
	} DataType;

public:
	static Receiver* getInstance();
	DataType guessDataType(const QByteArray& header);
	void processData(Receiver::DataType dataType, const QByteArray& buffer);

signals:
	void newMessage   (const QString& message);
	void userList     (const QByteArray& buffer);
	void photoResponse(const QByteArray& buffer);

private:
	static Receiver* instance;
};


// A TCP socket connected to the server
class Connection : public QTcpSocket
{
	Q_OBJECT

public:
	typedef enum {
		WaitingForGreeting,
		ReadingGreeting,
		ReadyForUse
	} ConnectionState;

public:
	static Connection* getInstance(QObject* parent = 0);

	Connection(QObject* parent = 0);
	QString getUserName() const { return userName; }
	void setUserName(const QString& name) { userName = name; }
	ConnectionState getState() const { return state; }

	void send(const QByteArray& header, const QByteArray& body = QByteArray("P"));
	void send(const QByteArray& header, const QList<QByteArray>& bodies);

protected:
	void timerEvent(QTimerEvent* timerEvent);

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

private:
	static Connection* instance;

	ConnectionState    state;
	Receiver::DataType dataType;
	QByteArray         buffer;
	int                numBytes;
	int                transferTimerID;
	QString            userName;
	Receiver*          receiver;
};

// Sends 4 types of header: REQUEST_USERLIST, REQEUST_PHOTO, REGISTER_PHOTO, EVENT
// Format of packet: header#size#body
// Format of body:
//		REQUEST_USERLIST: [empty], server knows the user name
//		REQEUST_PHOTO: target user name
//		REGISTER_PHOTO: file format#binary photo data
//		EVENT: event type#parameters
//			Format of parameters: parameter1#parameter2#...
class Sender : public QObject
{
public:
	static Sender* getInstance();
	Sender();
	void sendEvent(const QString& event, const QString& parameters);
	void sendUserListRequest();
	void sendPhotoRequest(const QString& targetUser);
	void sendPhotoRegistration(const QByteArray& format, const QByteArray& photoData);

private:
	static Sender* instance;
	Connection* connection;
};

#endif // CONNECTION_H
