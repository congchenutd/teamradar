#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QColor>
#include "PeerModel.h"

class Connection;
class PeerModel;

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
	PeerModel* getPeerModel() const { return model; }

signals:
	void userListChanged();

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
	PeerModel* model;
};


#endif // PEER_MANAGER_H
