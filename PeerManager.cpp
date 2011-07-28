#include "PeerManager.h"
#include "Setting.h"
#include "Connection.h"

PeerManager::PeerManager(QObject *parent)
	: QObject(parent)
{
	connection = Connection::getInstance();
	connect(connection, SIGNAL(readyForUse()),             this, SLOT(onConnected()));
	connect(connection, SIGNAL(userList(QByteArray)),      this, SLOT(onUserList(QByteArray)));
	connect(connection, SIGNAL(photoResponse(QByteArray)), this, SLOT(onPhotoResponse(QByteArray)));
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
	// copy to local
	QString fileName = userName + "." + QFileInfo(imagePath).suffix();
	if(fileName != imagePath)
	{
		QFile::remove(fileName);
		QFile::copy(imagePath, fileName);
	}

	Peers::Iterator it = peers.find(userName);
	if(it == peers.end())   // no such user, add one
	{
		DeveloperInfo developer(userName);
		developer.image = fileName;
		peers.insert(userName, developer);
	}
	else
		it->image = fileName;
}

QString PeerManager::getImage(const QString& userName) const
{
	Peers::Iterator it = peers.find(userName);
	return it == peers.end() ?
		Setting::getInstance()->value("DefaultDeveloperImage").toString() 
		: it->image;
}

QColor PeerManager::getDeveloperColor(const QString& userName)
{
	Peers::Iterator it = peers.find(userName);
	if(it == peers.end())
		return Setting::getInstance()->getColor("DefaultDeveloperColor");
	return it->color;
}

void PeerManager::setDeveloperColor(const QString& userName, const QColor& color)
{
	Peers::Iterator it = peers.find(userName);
	if(it == peers.end())
	{
		DeveloperInfo developer(userName);
		developer.color = color;
		peers.insert(userName, developer);
	}
	else
		it->color = color;
}

void PeerManager::onConnected() {
	refreshUserList();
}

void PeerManager::refreshUserList() {
	if(connection->getState() == Connection::ReadyForUse)
		connection->write("REQUEST_USERLIST#");
}

void PeerManager::onUserList(const QByteArray& list)
{
	QList<QByteArray> userNames = list.split(';');
	foreach(QString name, userNames)
	{
		if(!peers.contains(name))
			peers.insert(name, DeveloperInfo(name));
		requestPhoto(name);
	}
}

void PeerManager::requestPhoto(const QString& user)
{
	if(connection->getState() != Connection::ReadyForUse)
		return;
	connection->write("REQUEST_PHOTO#" + 
		QByteArray::number(user.length()) + "#" + 
		user.toUtf8());
}

// filename + # + filedata
void PeerManager::onPhotoResponse(const QByteArray& photoData)
{
	int seperator = photoData.indexOf('#');
	if(seperator == -1)
		return;

	QString fileName = photoData.left(seperator);
	QByteArray fileData = photoData.right(photoData.length() - seperator - 1);
	QFile file(fileName);
	if(file.open(QFile::WriteOnly | QFile::Truncate))
		file.write(fileData);

	QString userName = QFileInfo(fileName).baseName();
	setImage(userName, fileName);
}

Peers PeerManager::getPeersList() const {
	return peers;
}

void PeerManager::setColor(const QString& userName, const QColor& color)
{
	Peers::Iterator it = peers.find(userName);
	if(it == peers.end())
		it->color = color;
}

//////////////////////////////////////////////////////////////////////////
DeveloperInfo::DeveloperInfo(const QString& n) : name(n), online(true)
{
	image = Setting::getInstance()->value("DefaultDeveloperImage").toString();
	color = Setting::getInstance()->getColor("DefaultDeveloperColor");
}