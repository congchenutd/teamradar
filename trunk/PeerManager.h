#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QColor>
#include "PeerModel.h"

// Handles the status and configurations of peers
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
	void   refreshUserList();                            // ask the server for online user list
	PeerModel* getPeerModel() const { return model; }    // the underlying model

signals:
	void userListChanged(const QString& msg);

	// handle server messages
private slots:
	void onUserList(const QByteArray& list);
	void onPhotoResponse(const QByteArray& photoData);
	void onNewMessage(const QString& message);   // intercept CONNECTED/DISCONNECTED messages

private:
	PeerManager(QObject* parent = 0);
	void requestPhoto(const QString& user);
	void setUserOnline(const QString& name, bool online);  // update online status

private:
	static PeerManager* instance;
	PeerModel* model;
};


#endif // PEER_MANAGER_H
