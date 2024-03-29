#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include "ui_PlayerWidget.h"
#include <QIcon>
#include <QMap>
#include <QSet>

class QStandardItemModel;

namespace TeamRadar {

class PeerManager;
struct TeamRadarEvent;

class PlayerWidget : public QWidget
{
	Q_OBJECT

public:
	PlayerWidget(QWidget* parent = 0);
	~PlayerWidget();
	void reloadProject();
	void setDeveloperImage(const QString& name, const QImage& image);
	void setDeveloperColor(const QString& name, const QColor& color);

	static PlayerWidget* getInstance();

protected:
	void closeEvent (QCloseEvent*);
	void resizeEvent(QResizeEvent*);

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
	void onDownload();
	void onConnectedToServer(bool connected);
	void onEvent          (const TeamRadarEvent& event);
	void onEventDownloaded(const TeamRadarEvent& event);
	void onChatMessage(const QString& peerName, const QString& content);
	void onAnalyze();

private:
	void play(int row);
	void play(const TeamRadarEvent& event);
	void stop();
	void selectRow(int row);

private:
	enum {DateTime, UserName, EventType, Parameter};

	static PlayerWidget* instance;

	Ui::PlayerWidget ui;
	QIcon playIcon;
	QIcon pauseIcon;
	QStandardItemModel* modelPlaylist;
	double speed;
	int    currentRow;
	bool   online;
	bool   playing;
	PeerManager* peerManager;
};


} // namespace TeamRadar

#endif // PLAYERWIDGET_H
