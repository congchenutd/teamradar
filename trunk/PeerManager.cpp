#include "PeerManager.h"
#include "Setting.h"
#include "Connection.h"
#include "PeerModel.h"
#include "TeamRadarView.h"
#include "Defines.h"
#include <QFileInfo>
#include <QSqlError>

#ifdef OS_DESKTOP
	#include "PlayerWidget.h"
	#include "MessageCollector.h"
#endif

namespace TeamRadar {

PeerManager::PeerManager(QObject *parent)
	: QObject(parent), view(0)
{
	modelPeers = new PeerModel(this);
	modelPeers->setTable("Peers");
	modelPeers->select();
	modelPeers->removeRows(0, modelPeers->rowCount());

	sender = Sender::getInstance();

	Receiver* receiver = Receiver::getInstance();
	connect(receiver, SIGNAL(teamMembersReply(QList<QByteArray>)), this, SLOT(onTeamMembersReply(QList<QByteArray>)));
	connect(receiver, SIGNAL(onlineReply(QString, bool)),       this, SLOT(setUserOnline(QString, bool)));
	connect(receiver, SIGNAL(photoReply (QString, QByteArray)), this, SLOT(onPhotoReply (QString, QByteArray)));
	connect(receiver, SIGNAL(colorReply (QString, QByteArray)), this, SLOT(onColorReply (QString, QByteArray)));
	connect(receiver, SIGNAL(newEvent(TeamRadarEvent)), this, SLOT(onEvent(TeamRadarEvent)));

#ifdef OS_DESKTOP
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

QImage PeerManager::getImage(const QString& userName) const {
	return QImage(modelPeers->getUserInfo(userName).image);
}

QColor PeerManager::getDeveloperColor(const QString& userName) {
	return modelPeers->getUserInfo(userName).color;
}

void PeerManager::onTeamMembersReply(const QList<QByteArray>& list)
{
	foreach(const QString& name, list)
	{
		sender->sendOnlineRequest(name);
		sender->sendPhotoRequest(name);
		sender->sendColorRequest(name);
	}
}

// update the online status in the database, and notify player of the change
void PeerManager::setUserOnline(const QString& name, bool online)
{
	// update online info
	DeveloperInfo user = modelPeers->getUserInfo(name);
	user.online = online;
	PeerModel::updateUser(user);
	modelPeers->select();

	// update online users' locations
	if(online && Setting::getInstance()->getUserName() != name)
		sender->sendLocationRequest(name);

	// notify player
	QString event = online ? "CONNECTED" : "DISCONNECTED";
	emit userOnline(TeamRadarEvent(name, event));
}

void PeerManager::onPhotoReply(const QString& fileName, const QByteArray& photoData)
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
	DeveloperInfo userInfo = modelPeers->getUserInfo(userName);
	userInfo.image = filePath;
	PeerModel::updateUser(userInfo);
	modelPeers->select();

	// ask the node to use the new photo
#ifdef OS_DESKTOP
	PlayerWidget::getInstance()->setDeveloperImage(userName, QImage(filePath));
#endif
}

void PeerManager::onColorReply(const QString& userName, const QByteArray& color)
{
	DeveloperInfo userInfo = modelPeers->getUserInfo(userName);
	userInfo.color = QString(color);
	PeerModel::updateUser(userInfo);
	modelPeers->select();

	// update the color on the view
#ifdef OS_DESKTOP
	PlayerWidget::getInstance()->setDeveloperColor(userName, QColor(QString(color)));
#endif
}

void PeerManager::onEvent(const TeamRadarEvent& event)
{
	// peers
	if(event.eventType == "JOINED")
		setUserOnline(event.userName, true);
	else if(event.eventType == "DISCONNECTED")
		setUserOnline(event.userName, false);

	// local
#ifdef OS_DESKTOP
	else if(event.eventType == "OPENPROJECT")
	{
		Setting::getInstance()->setRootPath(event.parameters);
		Sender::getInstance()->sendJoinProject(QFileInfo(event.parameters).baseName());
		PlayerWidget::getInstance()->reloadProject();
		Sender::getInstance()->sendTeamMemberRequest();  // refresh member list
	}
	else if(event.eventType == "CLOSEPROJECT")
	{
		Setting::getInstance()->setRootPath(QString());
		PlayerWidget::getInstance()->reloadProject();
	}
#endif
}

} // namespace TeamRadar
