#include "OptionsPage.h"
#include "TeamRadarDlg.h"
#include <QLabel>

OptionsPage::OptionsPage(QObject* parent) : IOptionsPage(parent), dlg(0)
{}

QString OptionsPage::id() const {
	return tr("TeamRadar");
}

QString OptionsPage::trName() const {
	return tr("Team Radar");
}

QString OptionsPage::category() const {
	return tr("TeamRadar");
}

QString OptionsPage::trCategory() const {
	return category();
}

QString OptionsPage::displayName() const {
	return trName();
}

QIcon OptionsPage::categoryIcon() const {
	return QIcon(":/Images/Radar.png");
}

QString OptionsPage::displayCategory() const {
	return trCategory();
}

QWidget * OptionsPage::createPage(QWidget *parent) {
	dlg = new TeamRadarDlg(parent);
	return dlg;
}

void OptionsPage::apply() {
	dlg->save();
}

void OptionsPage::finish()
{
}



