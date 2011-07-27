#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QColor>

class Connection;

struct DeveloperInfo
{
	DeveloperInfo(const QString& name);
	QString name;
	QColor  color;
	bool    online; 
	QString image;
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
	void   refreshUserList();
	Peers  getPeersList() const;
	void   setColor(const QString& userName, const QColor& color);

private slots:
	void onConnected();
	void onUserList(const QByteArray& list);
	void onPhotoResponse(const QByteArray& photoData);

private:
	PeerManager(QObject* parent = 0);
	void requestPhoto(const QString& user);

private:
	static PeerManager* instance;
	Peers peers;
	Connection* connection;
};


#endif // PEER_MANAGER_H
