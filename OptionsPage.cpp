#include "OptionsPage.h"
#include "TeamRadarDlg.h"
#include <QLabel>

OptionsPage::OptionsPage(QObject* parent) : IOptionsPage(parent), dlg(0)
{}

QString OptionsPage::id() const {
	return tr("TeamRadar");
}

QString OptionsPage::category() const {
	return tr("TeamRadar");
}

QString OptionsPage::displayName() const {
	return tr("Team Radar");
}

QIcon OptionsPage::categoryIcon() const {
	return QIcon(":/Images/Radar.png");
}

QString OptionsPage::displayCategory() const {
	return category();
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



