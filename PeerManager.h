#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QColor>

class Connection;

struct DeveloperInfo
{
//	DeveloperInfo(const QString& img, const QColor& clr) : image(img), color(clr) {}
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
	void    setImage(const QByteArray& rawData);
	QString getImage(const QString& userName) const;
	QColor getDeveloperColor(const QString& userName);
	void   setDeveloperColor(const QString& userName, const QColor& color);

private slots:
	void onConnected();
	void onUserList(const QByteArray& list);
	void onPhotoResponse(const QByteArray& photoData);

private:
	PeerManager(QObject* parent = 0);
	void requestPhoto(const QString& user);

private:
	static PeerManager* instance;
	Peers developers;
	Connection* connection;
};


#endif // PEER_MANAGER_H
