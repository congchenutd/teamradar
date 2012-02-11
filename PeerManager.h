#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QColor>
#include "PeerModel.h"

// Handles the (login/out) status of both itself and peers, and configurations of peers
class PeerModel;
struct TeamRadarEvent;
class TeamRadarView;
class QSqlTableModel;

class PeerManager : public QObject
{
	Q_OBJECT

public:
	static PeerManager* getInstance();
	QString getImage(const QString& userName) const;
	QColor getDeveloperColor(const QString& userName);
	PeerModel*      getPeerModel()     const { return modelOnline; }  // the underlying models
	QSqlTableModel* getAllPeersModel() const { return modelAll; }
	void setView(TeamRadarView* v) { view = v; }

signals:
	void userOnline(const TeamRadarEvent& event);

// handle server messages
public slots:
	void refreshUserList();      // ask the server for online user, and all users list

private slots:
	void onUserList(const QList<QByteArray>& list);
	void onAllUsers(const QList<QByteArray>& list);
	void onPhotoResponse(const QString& fileName, const QByteArray& photoData);
	void onColorResponse(const QString& userName, const QByteArray& color);

	// handles CONNECTED, DISCONNECTED, JOINTED messages
	void onEvent(const TeamRadarEvent& event);

private:
	PeerManager(QObject* parent = 0);
	void setUserOnline(const QString& name, bool online);  // update online status

private:
	static PeerManager* instance;
	PeerModel*      modelOnline;   // online users
	QSqlTableModel* modelAll;      // all logged users
	TeamRadarView* view;
};


#endif // PEER_MANAGER_H
