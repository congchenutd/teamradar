#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QTimer>
#include <QTime>
#include <QStringList>

class Connection : public QTcpSocket
{
	Q_OBJECT

public:
	typedef enum {
		WaitingForGreeting,
		ReadingGreeting,
		ReadyForUse
	} ConnectionState;

	typedef enum {
		Undefined,
		Greeting,
		Ping,
		Pong,
		PhotoResponse,
		UserListResponse,
		Event,
		Connected,
		Disconnected
	} DataType;

public:
	static Connection* getInstance(QObject* parent = 0);

	Connection(QObject* parent = 0);
	~Connection() {}
	void setUserName(const QString& name) { userName = name; }
	ConnectionState getState() const { return state; }

	void send(const QString& header, const QString& body = QString("P"));
	void send(const QString& header, const QStringList& bodies);

protected:
	void timerEvent(QTimerEvent* timerEvent);

signals:
	void readyForUse();
	void newMessage(const QString& message);
	void userList(const QByteArray& buffer);
	void photoResponse(const QByteArray& buffer);
	void userConnected   (const QString& msg);
	void userDisconnected(const QString& msg);

private slots:
	void onReadyRead();
	void sendPing();
	void sendGreeting();
	void onDisconnected();

private:
	bool readHeader();
	int  readDataIntoBuffer(int maxSize = MaxBufferSize);
	int  getDataLength();
	bool hasEnoughData();
	void processData();
	DataType guessDataType(const QByteArray& header);
public:
	static const int  MaxBufferSize   = 1024 * 1024;
	static const int  TransferTimeout = 30 * 1000;
	static const int  PongTimeout     = 60 * 1000;
	static const int  PingInterval    = 10 * 1000;

private:
	static Connection* instance;

	QTimer          pingTimer;
	QTime           pongTime;
	ConnectionState state;
	DataType        dataType;
	QByteArray      buffer;
	int             numBytes;
	int             timerId;
	QString         userName;
};

#endif // CONNECTION_H
