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
	QImage     getImage(const QString& userName) const;
	QColor     getDeveloperColor(const QString& userName);
	PeerModel* getPeerModel() const { return modelPeers; }  // the underlying model
	void setView(TeamRadarView* v) { view = v; }

signals:
	void userOnline(const TeamRadarEvent& event);

// handle server messages
private slots:
	void onTeamMembersReply(const QList<QByteArray>& list);
	void onPhotoReply (const QString& fileName, const QByteArray& photoData);
	void onColorReply (const QString& userName, const QByteArray& color);
	void onEvent(const TeamRadarEvent& event);  // DISCONNECTED/JOINTED, OPEN/CLOSE PROJECT
	void setUserOnline(const QString& name, bool online);  // update online status

private:
	PeerManager(QObject* parent = 0);

private:
	static PeerManager* instance;
	PeerModel*     modelPeers;    // team members
	TeamRadarView* view;
	Sender*        sender;
};


#endif // PEER_MANAGER_H
