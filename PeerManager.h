#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QColor>

struct DeveloperInfo
{
	QString image;
	QColor  color;
};

typedef QMap<QString, DeveloperInfo> Peers;

class PeerManager : public QObject
{
	Q_OBJECT

public:
	static PeerManager* getInstance();
	void    setImage(const QString& userName, const QString& imagePath);
	QString getImage(const QString& userName) const;
	QColor getDeveloperColor(const QString& userName);
	void   setDeveloperColor(const QString& userName, const QColor& color);

private:
	PeerManager(QObject* parent = 0);
private:
	static PeerManager* instance;
	Peers developers;
};


#endif // PEER_MANAGER_H
