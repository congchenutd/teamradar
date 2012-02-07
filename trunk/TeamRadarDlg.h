#ifndef TEAMRADARWINDOW_H
#define TEAMRADARWINDOW_H

#include <QtGui/QWidget>
#include "ui_TeamRadarDlg.h"
#include "MySetting.h"

// Configuration dialog in the options page

class Setting;
class PeerManager;
class PeerModel;

class TeamRadarDlg : public QWidget
{
	Q_OBJECT

public:
	TeamRadarDlg(QWidget *parent = 0);
	void save();

protected:
	void contextMenuEvent(QContextMenuEvent*);

private slots:
	void resizeTable();
	void onDelete();
	void onShowHint();
	void onConnectedToServer(bool connected);

private:
	QString guessUserName() const;   // initialize user name
	QString getUserName()   const;   // convenient
	void    registerPhoto();

private:
	Ui::TeamRadarDlgClass ui;
	Setting*     setting;
	PeerModel*   model;
	PeerManager* peerManager;
};


#endif // TEAMRADARWINDOW_H
