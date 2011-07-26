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
	Peers::Iterator it = developers.find(userName);
	if(it == developers.end())
	{
		DeveloperInfo developer;
		developer.image = imagePath;
		developer.color = Setting::getInstance()->getColor("DefaultDeveloperColor");
		developers.insert(userName, developer);
	}
	else
		it->image = imagePath;
}

// filename + # + filedata
void PeerManager::setImage(const QByteArray& rawData)
{
	int seperator = rawData.indexOf('#');
	if(seperator == -1)
		return;

	QByteArray fileName = rawData.left(seperator);
	QByteArray fileData = rawData.right(rawData.length() - seperator - 1);
	QFile file(fileName);
	if(file.open(QFile::WriteOnly | QFile::Truncate))
		file.write(fileData);

	QString developerName = QFileInfo(fileName).baseName();
	setImage(developerName, fileName);
}

QString PeerManager::getImage(const QString& userName) const
{
	Peers::Iterator it = developers.find(userName);
	if(it == developers.end())
		return Setting::getInstance()->value("DefaultDeveloperImage").toString();
	return it->image;
}

QColor PeerManager::getDeveloperColor(const QString& userName)
{
	Peers::Iterator it = developers.find(userName);
	if(it == developers.end())
		return Setting::getInstance()->getColor("DefaultDeveloperColor");
	return it->color;
}

void PeerManager::setDeveloperColor(const QString& userName, const QColor& color)
{
	Peers::Iterator it = developers.find(userName);
	if(it == developers.end())
	{
		DeveloperInfo developer;
		developer.image = Setting::getInstance()->value("DefaultDeveloperImage").toString();
		developer.color = color;
		developers.insert(userName, developer);
	}
	else
		it->color = color;
}

void PeerManager::requestPhoto(const QString& user)
{
	if(connection->getState() != Connection::ReadyForUse)
		return;
	connection->write("REQUEST_PHOTO#" + 
					   QByteArray::number(user.length()) + "#" + 
					   user.toUtf8());
}

void PeerManager::onConnected()
{
	// request user list
	if(connection->getState() != Connection::ReadyForUse)
		return;
	connection->write("REQUEST_USERLIST#");
}

void PeerManager::onUserList(const QByteArray& list)
{
	developers.clear();
	QList<QByteArray> userNames = list.split(';');
	foreach(QString name, userNames)
	{
		developers.insert(name, DeveloperInfo());
		requestPhoto(name);
	}
}

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