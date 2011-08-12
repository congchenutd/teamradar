#include "PeerManager.h"
#include "Setting.h"
#include "Connection.h"
#include "PeerModel.h"

PeerManager::PeerManager(QObject *parent) : QObject(parent)
{
	model = new PeerModel(this);
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->setTable("Peers");
	model->select();

	Receiver* receiver = Receiver::getInstance();
	connect(receiver, SIGNAL(userList(QByteArray)),      this, SLOT(onUserList(QByteArray)));
	connect(receiver, SIGNAL(photoResponse(QByteArray)), this, SLOT(onPhotoResponse(QByteArray)));
	connect(receiver, SIGNAL(newMessage(QString)),       this, SLOT(onNewMessage(QString)));
}

PeerManager* PeerManager::getInstance()
{
	if(instance == 0)
		instance = new PeerManager;
	return instance;
}

PeerManager* PeerManager::instance = 0;

void PeerManager::setImage(const QString& userName, const QString& imagePath)
{
	// copy image to local
	QString fileName = userName + "." + QFileInfo(imagePath).suffix();
	if(fileName != imagePath)
	{
		QFile::remove(fileName);
		QFile::copy(imagePath, fileName);
	}

	DeveloperInfo userInfo = model->getUserInfo(userName);
	userInfo.image = fileName;
	model->updateUser(userInfo);
}

QString PeerManager::getImage(const QString& userName) const {
	return model->getUserInfo(userName).image;
}

QColor PeerManager::getDeveloperColor(const QString& userName) {
	return model->getUserInfo(userName).color;
}

void PeerManager::setDeveloperColor(const QString& userName, const QColor& color)
{
	DeveloperInfo userInfo = model->getUserInfo(userName);
	userInfo.color = color;
	model->updateUser(userInfo);
}

void PeerManager::refreshUserList() {
	Sender::getInstance()->sendUserListRequest();
}

// online user list
void PeerManager::onUserList(const QByteArray& list)
{
	model->makeAllOffline();
	QList<QByteArray> userNames = list.split('#');
	foreach(QString name, userNames)
		updateUser(name, true);
}

void PeerManager::updateUser(const QString& name, bool online)
{
	// update online info
	DeveloperInfo user = model->getUserInfo(name);
	user.online = online;
	model->updateUser(user);

	// update photo
	if(online && Setting::getInstance()->getUserName() != name)
		requestPhoto(name);
	model->select();

	// notify player
	QString msg(name + '#');
	msg += online ? "CONNECTED#" : "DISCONNECTED";
	emit userListChanged(msg);
}

void PeerManager::requestPhoto(const QString& user) {
	Sender::getInstance()->sendPhotoRequest(user);
}

// filename + # + filedata
void PeerManager::onPhotoResponse(const QByteArray& photoData)
{
	// parse data
	int seperator = photoData.indexOf('#');
	if(seperator == -1)
		return;

	QString fileName = photoData.left(seperator);
	QByteArray fileData = photoData.right(photoData.length() - seperator - 1);

	// save photo file
	QFile file(fileName);
	if(file.open(QFile::WriteOnly | QFile::Truncate))
		file.write(fileData);

	QString userName = QFileInfo(fileName).baseName();
	setImage(userName, fileName);
}

// user#event#parameters
void PeerManager::onNewMessage(const QString& message)
{
	QString user  = message.split('#').at(0);
	QString event = message.split('#').at(1);
	if(event == "CONNECTED")
		updateUser(user, true);
	else if(event == "DISCONNECTED")
		updateUser(user, false);
}