#include "PeerManager.h"
#include "Setting.h"
#include "Connection.h"
#include "PeerModel.h"

PeerManager::PeerManager(QObject *parent) : QObject(parent)
{
	model = new PeerModel(this);
	model->setColumnType(model->NAME,    model->NameColumn);
	model->setColumnType(model->COLOR,   model->ColorColumn);
	model->setColumnType(model->IMAGE,   model->ImageColumn);
	model->setColumnType(model->ONLINE,  model->BoolColumn);
	model->setColumnType(model->RECEIVE, model->BoolColumn);
	model->setGrayImageBy(model->ONLINE);
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->setTable("Peers");
	model->select();

	Receiver* receiver = Receiver::getInstance();
	connect(receiver, SIGNAL(userList(QList<QByteArray>)),        this, SLOT(onUserList(QList<QByteArray>)));
	connect(receiver, SIGNAL(photoResponse(QString, QByteArray)), this, SLOT(onPhotoResponse(QString, QByteArray)));
	connect(receiver, SIGNAL(colorResponse(QString, QByteArray)), this, SLOT(onColorResponse(QString, QByteArray)));
	connect(receiver, SIGNAL(newEvent(TeamRadarEvent)),           this, SLOT(onEvent(TeamRadarEvent)));
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

void PeerManager::setImage(const QString& userName, const QString& imagePath)
{
	// copy image to local
	QString fileName = userName + "." + QFileInfo(imagePath).suffix();
	if(fileName != imagePath)
	{
		QFile::remove(fileName);
		QFile::copy(imagePath, fileName);
	}

	// update the db
	DeveloperInfo userInfo = model->getUserInfo(userName);
	userInfo.image = fileName;
	model->updateUser(userInfo);
	model->select();
}

void PeerManager::setDeveloperColor(const QString& userName, const QColor& color)
{
	DeveloperInfo userInfo = model->getUserInfo(userName);
	userInfo.color = color;
	model->updateUser(userInfo);
	model->select();
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
	QString event = online ? "CONNECTED#" : "DISCONNECTED#";
	emit userOnline(TeamRadarEvent(name, event));
}

void PeerManager::onPhotoResponse(const QString& fileName, const QByteArray& photoData)
{
	// save photo file
	QFile file(fileName);
	if(file.open(QFile::WriteOnly | QFile::Truncate))
		file.write(photoData);

	QString userName = QFileInfo(fileName).baseName();
	setImage(userName, fileName);
}

void PeerManager::onColorResponse(const QString& userName, const QByteArray& color) {
	setDeveloperColor(userName, QString(color));
}

void PeerManager::onEvent(const TeamRadarEvent& event)
{
	if(event.eventType == "CONNECTED")
		setUserOnline(event.userName, true);
	else if(event.eventType == "DISCONNECTED")
		setUserOnline(event.userName, false);
}