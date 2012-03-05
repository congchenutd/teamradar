#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QTimer>
#include <QTime>
#include <QStringList>
#include <QMap>
#include "TeamRadarEvent.h"

// Parses the message header & body from Connection
// Format of packet: header#size#body
class Receiver : public QObject
{
	Q_OBJECT

public:
	typedef enum {
		Undefined,         // Format of body see below:
		Greeting,          // GREETING: [OK, CONNECTED]/[WRONG_USER]
		Chat,
		Event,             // EVENT: user#event#[parameters]#time
						   //	  Format of parameters: parameter1#parameter2#...
		EventsReply,       // same as EVENT
		TeamMembersReply,  // ALLUSERS_REPLY: username1#username2#...
		PhotoReply,        // PHOTO_REPLY: [filename#binary photo data]/[empty]
		ColorReply,        // COLOR_REPLY: [username#color]/[empty]
		OnlineReply,       // targetUser#online
		TimeSpanReply,     // TIMESPAN_REPLY: start#end
		ProjectsReply      // PROJECTS_REPLY: projectName1#name2...
	} DataType;

	typedef void(Receiver::*Parser)(const QByteArray& buffer);

public:
	static Receiver* getInstance();

	Receiver();
	DataType guessDataType(const QByteArray& header);
	void processData(Receiver::DataType dataType, const QByteArray& buffer);

signals:
	void newEvent   (const TeamRadarEvent& event);
	void eventsReply(const TeamRadarEvent& event);
	void teamMembersReply(const QList<QByteArray>& list);
	void onlineReply(const QString& targetUser, bool online);
	void colorReply (const QString& targetUser, const QByteArray& color);
	void photoReply (const QString& fileName,   const QByteArray& photoData);
	void chatMessage(const QString& peerName, const QString& content);
	void timespanReply(const QDateTime& start, const QDateTime& end);
	void projectsReply(const QStringList& list);

private:
	void parseGreeting        (const QByteArray& buffer);
	void parseChat            (const QByteArray& buffer);
	void parseEvent           (const QByteArray& buffer);
	void parseEventsReply     (const QByteArray& buffer);
	void parseTeamMembersReply(const QByteArray& buffer);
	void parseOnlinerReply    (const QByteArray& buffer);
	void parsePhotoReply      (const QByteArray& buffer);
	void parseColorReply      (const QByteArray& buffer);
	void parseTimeSpanReply   (const QByteArray& buffer);
	void parseProjectsReply   (const QByteArray& buffer);

private:
	static Receiver* instance;
	QMap<QString, DataType> dataTypes;
	QMap<DataType, Parser>  parsers;
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
	bool    isReadyForUse() const { return state() == ConnectedState && ready;    }
	void setUserName(const QString& name);
	void setReadyForUse();

	void send(const QByteArray& header, const QByteArray& body = QByteArray());
	void send(const QByteArray& header, const QList<QByteArray>& bodies);
	void reconnect();

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
// See TeamRadarServer's connection.h for details of the protocol
class Sender : public QObject
{
public:
	static Sender* getInstance();
	Sender();
	void sendEvent(const QString& event, const QString& parameters);
	void sendTeamMemberRequest();
	void sendPhotoRegistration(const QByteArray& format, const QByteArray& photoData);
	void sendColorRegistration(const QColor& color);
	void sendOnlineRequest(const QString& targetUser);
	void sendPhotoRequest (const QString& targetUser);
	void sendColorRequest (const QString& targetUser);
	void sendEventRequest(const QStringList& users, const QStringList& eventTypes,
						  const QDateTime& startTime, const QDateTime& endTime,
						  const QStringList& phases, int fuzziness);
	void sendChat(const QStringList& recipients, const QString& content);
	void sendTimeSpanRequest();
	void sendProjectsRequest();
	void sendJoinProject(const QString& projectName);
	void sendLocationRequest(const QString& userName);

private:
	static Sender* instance;
	Connection* connection;
};

#endif // CONNECTION_H
