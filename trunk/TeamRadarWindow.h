#ifndef TEAMRADARWINDOW_H
#define TEAMRADARWINDOW_H

#include <QtGui/QDialog>
#include "ui_TeamRadarDlg.h"
#include "MySetting.h"
#include "PeerModel.h"
	
class Setting;
class PeerManager;

class TeamRadarWindow : public QDialog
{
    Q_OBJECT

public:
    TeamRadarWindow(QWidget *parent = 0);
    void accept();

private slots:
    void onSetImage();
	void onRefresh();
	void onEditPeer(const QModelIndex& idx);
	void onUserListChanged();

private:
	QString guessUserName() const;
	void registerPhoto();
	void resizeTable();

public:
	enum {PEER_NAME, PEER_COLOR, PEER_IMAGE, PEER_ONLINE};

private:
	Ui::TeamRadarDlgClass ui;
	Setting* setting;
	QString userName;
	bool imageChanged;
	PeerModel model;
	PeerManager* peerManager;
};


#endif // TEAMRADARWINDOW_H
