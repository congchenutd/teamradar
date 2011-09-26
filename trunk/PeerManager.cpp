#include "PeerManager.h"
#include "Setting.h"
#include "Connection.h"
#include "PeerModel.h"
#include <QFileInfo>

#if !defined(Q_WS_SIMULATOR) && !defined(Q_OS_SYMBIAN)
#include "PlayerWidget.h"
#include "MessageCollector.h"
#endif

PeerManager::PeerManager(QObject *parent) : QObject(parent)
{
	model = new PeerModel(this);
	model->setTable("Peers");
	model->select();

	Receiver* receiver = Receiver::getInstance();
	connect(receiver, SIGNAL(userList(QList<QByteArray>)),        this, SLOT(onUserList(QList<QByteArray>)));
	connect(receiver, SIGNAL(photoResponse(QString, QByteArray)), this, SLOT(onPhotoResponse(QString, QByteArray)));
	connect(receiver, SIGNAL(colorResponse(QString, QByteArray)), this, SLOT(onColorResponse(QString, QByteArray)));
	connect(receiver, SIGNAL(newEvent(TeamRadarEvent)), this, SLOT(onEvent(TeamRadarEvent)));
	connect(Connection::getInstance(), SIGNAL(connectionStatusChanged(bool)), this, SLOT(refreshUserList()));

#if !defined(Q_WS_SIMULATOR) && !defined(Q_OS_SYMBIAN)
	connect(MessageCollector::getInstance(), SIGNAL(localEvent(TeamRadarEvent)), this, SLOT(onEvent(TeamRadarEvent)));
#endif
}

PeerManager* PeerManager::getInstance()
{
	if(instance == 0)
		instance = new PeerManager;
	return instance;
}

PeerManager* PeerManager::instance = 0;

QString PeerManager::getImage(const QString& userName) const {
	return model->getUserInfo(userName).image;
}

QColor PeerManager::getDeveloperColor(const QString& userName) {
	return model->getUserInfo(userName).color;
}

void PeerManager::refreshUserList() {
	Sender::getInstance()->sendUserListRequest();
}

// online user list
void PeerManager::onUserList(const QList<QByteArray>& list)
{
	model->makeAllOffline();
	foreach(QString name, list)
		setUserOnline(name, true);
}

// update the online status in the database, and notify player of the change
void PeerManager::setUserOnline(const QString& name, bool online)
{
	// update online info
	DeveloperInfo user = model->getUserInfo(name);
	user.online = online;
	model->updateUser(user);
	model->select();

	// update photo, color
	if(online && Setting::getInstance()->getUserName() != name)
	{
		Sender::getInstance()->sendPhotoRequest(name);
		Sender::getInstance()->sendColorRequest(name);
	}

	// notify player
	QString event = online ? "CONNECTED" : "DISCONNECTED";
	emit userOnline(TeamRadarEvent(name, event));
}

void PeerManager::onPhotoResponse(const QString& fileName, const QByteArray& photoData)
{
	// save photo file
	QString userName = QFileInfo(fileName).baseName();
	QString filePath = Setting::getInstance()->getPhotoFilePath(userName);
	QFile file(filePath);
	if(file.open(QFile::WriteOnly | QFile::Truncate))
	{
		file.write(photoData);
		file.close();
	}

	// update the db
	DeveloperInfo userInfo = model->getUserInfo(userName);
	userInfo.image = filePath;
	model->updateUser(userInfo);
	model->select();

	// ask the node to use the new photo
	PlayerWidget::getInstance()->reloadPhoto(userName);
}

void PeerManager::onColorResponse(const QString& userName, const QByteArray& color)
{
	DeveloperInfo userInfo = model->getUserInfo(userName);
	userInfo.color = QString(color);
	model->updateUser(userInfo);
	model->select();
}

void PeerManager::onEvent(const TeamRadarEvent& event)
{
	if(event.eventType == "CONNECTED")
		setUserOnline(event.userName, true);
	else if(event.eventType == "DISCONNECTED")
		setUserOnline(event.userName, false);
#if !defined(Q_WS_SIMULATOR) && !defined(Q_OS_SYMBIAN)	
	else if(event.eventType == "OPENPROJECT")  // must be local
	{
		Setting::getInstance()->setRootPath(event.parameters);
		Sender::getInstance()->sendJoinProject(QFileInfo(event.parameters).baseName());
		PlayerWidget::getInstance()->reloadProject();
	}
#endif
}
