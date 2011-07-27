#ifndef TEAMRADARWINDOW_H
#define TEAMRADARWINDOW_H

#include <QtGui/QDialog>
#include "ui_TeamRadarDlg.h"
#include "MySetting.h"
#include "PeerModel.h"
	
class Setting;

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

private:
	QString guessUserName() const;
	void registerPhoto();

public:
	enum {PEER_NAME, PEER_COLOR, PEER_IMAGE};

private:
	Ui::TeamRadarDlgClass ui;
	Setting* setting;
	QString userName;
	bool imageChanged;
	PeerModel model;
};


#endif // TEAMRADARWINDOW_H
