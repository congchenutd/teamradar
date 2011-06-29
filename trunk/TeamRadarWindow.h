#ifndef TEAMRADARWINDOW_H
#define TEAMRADARWINDOW_H

#include <QtGui/QDialog>
#include "ui_TeamRadarDlg.h"
#include "MySetting.h"
	
class Setting;

class TeamRadarWindow : public QDialog
{
    Q_OBJECT

public:
    TeamRadarWindow(QWidget *parent = 0);
    void accept();

private slots:
    void onSetImage();

private:
	QString guessUserName() const;

private:
	Ui::TeamRadarDlgClass ui;
	Setting* setting;
	QString userName;
	bool imageChanged;
};


#endif // TEAMRADARWINDOW_H
