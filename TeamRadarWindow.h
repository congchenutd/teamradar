#ifndef TEAMRADARWINDOW_H
#define TEAMRADARWINDOW_H

#include <QtGui/QDialog>
#include "ui_TeamRadarDlg.h"
#include "MySetting.h"

namespace TeamRadar {
	
class UserSetting;

class TeamRadarWindow : public QDialog
{
    Q_OBJECT

public:
    TeamRadarWindow(QWidget *parent = 0);
	void accept();

private:
	QString guessUserName() const;

private:
	Ui::TeamRadarDlgClass ui;
	UserSetting* setting;
};


class UserSetting : public MySetting<UserSetting>
{
public:
	UserSetting(const QString& fileName);

	QString getServerAddress() const;
	quint16 getServerPort()    const;
	QString getUserName()      const;

	void setServerAddress(const QString& address);
	void setServerPort(quint16 port);
	void setUserName(const QString& name);

private:
	void loadDefaults();
};

} // namespace TeamRadar

#endif // TEAMRADARWINDOW_H
