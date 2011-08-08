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

	connection = Connection::getInstance();
	connect(connection, SIGNAL(readyForUse()),             this, SLOT(onConnected()));
	connect(connection, SIGNAL(userList(QByteArray)),      this, SLOT(onUserList(QByteArray)));
	connect(connection, SIGNAL(photoResponse(QByteArray)), this, SLOT(onPhotoResponse(QByteArray)));
	connect(connection, SIGNAL(userConnected   (QString)), this, SLOT(onUserConnected   (QString)));
	connect(connection, SIGNAL(userDisconnected(QString)), this, SLOT(onUserDisconnected(QString)));
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

void PeerManager::onConnected() {
	refreshUserList();
}

void PeerManager::refreshUserList() {
	if(connection->getState() == Connection::ReadyForUse)
		connection->write("REQUEST_USERLIST#" + QByteArray::number(1) + '#' + 'U');
}

// online user list
void PeerManager::onUserList(const QByteArray& list)
{
	model->makeAllOffline();
	QList<QByteArray> userNames = list.split(';');
	foreach(QString name, userNames)
		updateUser(name, true);
}

void PeerManager::updateUser(const QString& name, bool online)
{
	DeveloperInfo user = model->getUserInfo(name);
	user.online = online;
	model->updateUser(user);
	if(online)
		requestPhoto(name);
	model->select();

	QString msg(name + '#');
	if(online)
		msg += "CONNECTED#";
	else
		msg += "DISCONNECTED#";
	emit userListChanged(msg);
}

void PeerManager::requestPhoto(const QString& user) {
	if(connection->getState() == Connection::ReadyForUse)
		connection->write("REQUEST_PHOTO#" + 
			QByteArray::number(user.length()) + "#" + user.toUtf8());
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

void PeerManager::onUserConnected(const QString& name) {
	updateUser(name.split('#').front(), true);
}

void PeerManager::onUserDisconnected(const QString& name) {
	updateUser(name.split('#').front(), false);
}