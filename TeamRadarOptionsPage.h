#ifndef OPTIONSPAGE_H
#define OPTIONSPAGE_H

#include <coreplugin/dialogs/ioptionspage.h>

namespace TeamRadar {

class TeamRadarDlg;

class TeamRadarOptionsPage : public Core::IOptionsPage
{
	Q_OBJECT

public:
	TeamRadarOptionsPage(QObject* parent = 0);

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

class TeamRadarAboutPage : public TeamRadarOptionsPage
{
public:
	TeamRadarAboutPage(QObject* parent = 0);

	QString  id() const { return tr("Tab2"); }
	QString  displayName() const { return tr("About"); }
	QWidget* createPage(QWidget* parent);
	void apply() {}
};


} // namespace TeamRadar

#endif // OPTIONSPAGE_H
