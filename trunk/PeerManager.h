#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QColor>
#include "PeerModel.h"

// Handles the (login/out) status of both itself and peers, and configurations of peers
struct TeamRadarEvent;
class PeerModel;
class TeamRadarView;
class Sender;

class PeerManager : public QObject
{
	Q_OBJECT

public:
	static PeerManager* getInstance();
	QString getImage(const QString& userName) const;
	QColor getDeveloperColor(const QString& userName);
	PeerModel*      getPeerModel()     const { return modelPeers; }  // the underlying models
	void setView(TeamRadarView* v) { view = v; }

signals:
	void userOnline(const TeamRadarEvent& event);

// handle server messages
public slots:
	void refreshUserList();      // ask the server for online user, and all users list

private slots:
	void onAllUsers(const QList<QByteArray>& list);
	void onPhotoReply (const QString& fileName, const QByteArray& photoData);
	void onColorReply (const QString& userName, const QByteArray& color);
	void onOnlineReply(const QString& targetUser, bool online);

	// handles CONNECTED, DISCONNECTED, JOINTED messages
	void onEvent(const TeamRadarEvent& event);

private:
	PeerManager(QObject* parent = 0);
	void setUserOnline(const QString& name, bool online);  // update online status

private:
	static PeerManager* instance;
	PeerModel*      modelPeers;    // teammates
	TeamRadarView* view;
	Sender* sender;
};


#endif // PEER_MANAGER_H
