#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include "ui_PlayerWidget.h"
#include <QIcon>
#include <QMap>
#include <QSet>

class QStandardItemModel;
class PeerManager;
struct TeamRadarEvent;

class PlayerWidget : public QWidget
{
	Q_OBJECT

public:
	PlayerWidget(QWidget *parent = 0);

protected:
	virtual void closeEvent(QCloseEvent*);
	virtual void resizeEvent(QResizeEvent*);

public slots:
	void play();

private slots:
	void onShowPlaylist(bool show);
	void onPlayPause();
	void onLoad();
	void onSpeed(double speed);
	void onPlaylistClicked      (const QModelIndex& idx);
	void onPlaylistCoubleClicked(const QModelIndex& idx);
	void onOnline();
	void onEffects(bool show);
	void onNewMessage(const QString& message);

private:
	void play(int row);
	void play(const TeamRadarEvent& event);
	void stop();
	void selectRow(int row);

private:
	enum {DateTime, UserName, EventType, Parameter};

	Ui::PlayerWidget ui;
	QIcon playIcon;
	QIcon pauseIcon;
	QStandardItemModel* model;
	double speed;
	int    currentRow;
	bool   online;
	bool   playing;
	PeerManager* peerManager;
};


#endif // PLAYERWIDGET_H