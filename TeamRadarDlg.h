#ifndef TEAMRADARWINDOW_H
#define TEAMRADARWINDOW_H

#include <QtGui/QDialog>
#include "ui_TeamRadarDlg.h"
#include "MySetting.h"
	
// Configuration dialog

class Setting;
class PeerManager;
class PeerModel;

class TeamRadarDlg : public QDialog
{
    Q_OBJECT

public:
    TeamRadarDlg(QWidget *parent = 0);
    void accept();

protected:
	void contextMenuEvent(QContextMenuEvent*);

private slots:
    void onSetImage();
	void onSetColor();
	void resizeTable();
	void onDelete();

private:
	QString guessUserName() const;
	void registerPhoto();
	void registerColor();
	QString getUserName() const;
	void setColor(const QColor& color);

private:
	Ui::TeamRadarDlgClass ui;
	Setting*     setting;
	QColor       color;
	PeerModel*   model;
	PeerManager* peerManager;
};


#endif // TEAMRADARWINDOW_H
