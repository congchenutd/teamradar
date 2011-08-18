#ifndef TEAMRADARWINDOW_H
#define TEAMRADARWINDOW_H

#include <QtGui/QDialog>
#include "ui_TeamRadarDlg.h"
#include "MySetting.h"
	
// Configuration dialog

class Setting;
class PeerManager;
class PeerModel;

class TeamRadarWindow : public QDialog
{
    Q_OBJECT

public:
    TeamRadarWindow(QWidget *parent = 0);
    void accept();

private slots:
    void onSetImage();
	void onSetColor();

private:
	QString guessUserName() const;
	void registerPhoto();
	void registerColor();
	void resizeTable();
	QString getUserName() const;
	void setColor(const QColor& color);

public:
	enum {PEER_NAME, PEER_COLOR, PEER_IMAGE, PEER_ONLINE};

private:
	Ui::TeamRadarDlgClass ui;
	Setting*     setting;
	QColor       color;
	PeerModel*   model;
	PeerManager* peerManager;
};


#endif // TEAMRADARWINDOW_H
