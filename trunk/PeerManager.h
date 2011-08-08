#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QColor>
#include "PeerModel.h"

class Connection;
class PeerModel;

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
	PeerModel* getPeerModel() const { return model; }

signals:
	void userListChanged(const QString& msg);

private slots:
	void onConnected();
	void onUserList(const QByteArray& list);
	void onPhotoResponse(const QByteArray& photoData);
	void onUserConnected   (const QString& name);
	void onUserDisconnected(const QString& name);

private:
	PeerManager(QObject* parent = 0);
	void requestPhoto(const QString& user);
	void updateUser(const QString& name, bool online);

private:
	static PeerManager* instance;
	Connection* connection;
	PeerModel* model;
};


#endif // PEER_MANAGER_H
