#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QColor>
#include "PeerModel.h"

// Handles the status and configurations of peers
class PeerModel;
struct TeamRadarEvent;

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
	void userOnline(const TeamRadarEvent& event);

// handle server messages
private slots:
	void onUserList(const QList<QByteArray>& list);
	void onPhotoResponse(const QString& fileName, const QByteArray& photoData);
	void onColorResponse(const QString& userName, const QByteArray& color);
	void onEvent(const TeamRadarEvent& event);   // intercept CONNECTED/DISCONNECTED messages

private:
	PeerManager(QObject* parent = 0);
	void setUserOnline(const QString& name, bool online);  // update online status

private:
	static PeerManager* instance;
	PeerModel* model;
};


#endif // PEER_MANAGER_H
