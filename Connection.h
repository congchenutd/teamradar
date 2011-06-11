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
		RegisterResponse,
		PhotoResponse,
		Event,
	} DataType;

public:
	static Connection* getInstance(QObject* parent = 0);

	Connection(QObject* parent = 0);
	~Connection() {}
	void setUserName(const QString& name) { userName = name; }
	ConnectionState getState() const { return state; }

	void registerUser();
	void registerPhoto(const QString& photoPath);
	void requestPhotos();

protected:
	void timerEvent(QTimerEvent* timerEvent);

signals:
	void readyForUse();
	void newMessage(const QString &message);

private slots:
	void onReadyRead();
	void sendPing();
	void sendGreeting();

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
	bool            isGreetingSent;
	QString         userName;
};

#endif // CONNECTION_H
