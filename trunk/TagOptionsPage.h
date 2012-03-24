#ifndef SETTINGPAGE_H
#define SETTINGPAGE_H

#include <QIcon>
#include <coreplugin/dialogs/ioptionspage.h>
#include "TagOptionsDlg.h"

namespace TeamRadar {

class TagOptionsPage: public Core::IOptionsPage
{
	Q_OBJECT
public:

	TagOptionsPage(QObject* parent = 0) : IOptionsPage(parent) {}

	virtual QString id()              const { return "TodoSettingTab1_Setting"; }
	virtual QString category()        const { return "TODO"; }
	virtual QString displayName()     const { return "TODO"; }
	virtual QString displayCategory() const { return "TODO"; }
	virtual QIcon   categoryIcon()    const { return QIcon(":/Images/Tag"); }
	virtual QWidget* createPage(QWidget *parent);
	virtual void apply();
	virtual void finish() {}

public slots:
	void onSettingChanged();

private:
	TagOptionsDlg* dialog;
	bool        settingDirty;
};

class TagAboutPage : public TagOptionsPage
{
public:
	TagAboutPage(QObject* parent = 0);

	QString  id() const { return tr("TodoSettingTab2_About"); }
	QString  displayName() const { return tr("About"); }
	QWidget* createPage(QWidget* parent);
	void apply() {}
};

}  // namespace TeamRadar

#endif // SETTINGPAGE_H
