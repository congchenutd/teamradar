#include "PeerManager.h"
#include "Setting.h"

PeerManager::PeerManager(QObject *parent)
	: QObject(parent) {}

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
