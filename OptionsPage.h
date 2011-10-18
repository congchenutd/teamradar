#ifndef OPTIONSPAGE_H
#define OPTIONSPAGE_H

#include <coreplugin/dialogs/ioptionspage.h>

class TeamRadarDlg;

class OptionsPage : public Core::IOptionsPage
{
	Q_OBJECT

public:
	OptionsPage(QObject* parent = 0);

	QString id() const;
	QString displayName() const;
	QString category() const;
	QString displayCategory() const;
	QIcon categoryIcon() const;
	QWidget *createPage(QWidget *parent);
	void apply();
	void finish();

private:
	TeamRadarDlg* dlg;
};

#endif // OPTIONSPAGE_H
