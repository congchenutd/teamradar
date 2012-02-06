#ifndef OPTIONSPAGE_H
#define OPTIONSPAGE_H

#include <coreplugin/dialogs/ioptionspage.h>

class TeamRadarDlg;

class OptionsPage : public Core::IOptionsPage
{
	Q_OBJECT

public:
	OptionsPage(QObject* parent = 0);

	QString id()              const { return tr("Tab1"); }
	QString displayName()     const { return tr("Team Radar"); }
	QString category()        const { return tr("Team Radar"); }
	QString displayCategory() const { return category(); }
	QIcon categoryIcon() const;
	QWidget *createPage(QWidget *parent);
	void apply();
	void finish() {}

private:
	TeamRadarDlg* dlg;
};

class AboutOptionPage : public OptionsPage
{
public:
	AboutOptionPage(QObject* parent = 0);

	QString  id() const { return tr("Tab2"); }
	QString  displayName() const { return tr("About"); }
	QWidget* createPage(QWidget* parent);
};


#endif // OPTIONSPAGE_H
