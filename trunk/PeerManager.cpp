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

PeerManager::PeerManager(QObject *parent)
	: QObject(parent), view(0)
{
	modelPeers = new PeerModel(this);
	modelPeers->setTable("Peers");
	modelPeers->select();

	modelAll = new QSqlTableModel(this);
	modelAll->setEditStrategy(QSqlTableModel::OnManualSubmit);
	modelAll->setTable("AllPeers");
	modelAll->select();

	Receiver* receiver = Receiver::getInstance();
	connect(receiver, SIGNAL(userList(QList<QByteArray>)), this, SLOT(onUserList(QList<QByteArray>)));
	connect(receiver, SIGNAL(allUsers(QList<QByteArray>)), this, SLOT(onAllUsers(QList<QByteArray>)));
	connect(receiver, SIGNAL(photoResponse(QString, QByteArray)), this, SLOT(onPhotoResponse(QString, QByteArray)));
	connect(receiver, SIGNAL(colorResponse(QString, QByteArray)), this, SLOT(onColorResponse(QString, QByteArray)));
	connect(receiver, SIGNAL(newEvent(TeamRadarEvent)), this, SLOT(onEvent(TeamRadarEvent)));
//	connect(Connection::getInstance(), SIGNAL(connectionStatusChanged(bool)), this, SLOT(refreshUserList()));

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

QString PeerManager::getImage(const QString& userName) const {
	return modelPeers->getUserInfo(userName).image;
}

QColor PeerManager::getDeveloperColor(const QString& userName) {
	return modelPeers->getUserInfo(userName).color;
}

void PeerManager::refreshUserList() {
	Sender::getInstance()->sendAllUsersRequest();
	Sender::getInstance()->sendUserListRequest();
}

// teammates list
void PeerManager::onUserList(const QList<QByteArray>& list)
{
	modelPeers->makeAllOffline();
	foreach(QString name, list)
		setUserOnline(name, true);
}

// update all users model
void PeerManager::onAllUsers(const QList<QByteArray>& list)
{
	modelAll->removeRows(0, modelAll->rowCount());
	foreach(QByteArray userName, list)
	{
		int lastRow = modelAll->rowCount();
		modelAll->insertRow(lastRow);
		modelAll->setData(modelAll->index(lastRow, 0), userName);
		DeveloperInfo developerInfo = PeerModel::getUserInfo(userName);
		modelAll->setData(modelAll->index(lastRow, 1), developerInfo.image);
		modelAll->setData(modelAll->index(lastRow, 2), true);
	}
	modelAll->submitAll();
}

// update the online status in the database, and notify player of the change
void PeerManager::setUserOnline(const QString& name, bool online)
{
	// update online info
	DeveloperInfo user = modelPeers->getUserInfo(name);
	user.online = online;
	PeerModel::updateUser(user);
	modelPeers->select();

	// update photo, color
	if(online && Setting::getInstance()->getUserName() != name)
	{
		Sender::getInstance()->sendPhotoRequest(name);
		Sender::getInstance()->sendColorRequest(name);
		Sender::getInstance()->sendLocationRequest(name);
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
	DeveloperInfo userInfo = modelPeers->getUserInfo(userName);
	userInfo.image = filePath;
	PeerModel::updateUser(userInfo);
	modelPeers->select();

	// ask the node to use the new photo
	if(view != 0)
		view->reloadDeveloperImage(userName, QImage(filePath));
}

void PeerManager::onColorResponse(const QString& userName, const QByteArray& color)
{
	DeveloperInfo userInfo = modelPeers->getUserInfo(userName);
	userInfo.color = QString(color);
	PeerModel::updateUser(userInfo);
	modelPeers->select();
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
		refreshUserList();   // refresh users in the same project
	}
	else if(event.eventType == "CLOSEPROJECT")
	{
		Setting::getInstance()->setRootPath(QString());
		PlayerWidget::getInstance()->reloadProject();
	}
#endif
}
