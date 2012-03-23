#include "OptionsPage.h"
#include "TeamRadarDlg.h"
#include "Setting.h"
#include <QLabel>

namespace TeamRadar {

OptionsPage::OptionsPage(QObject* parent) : IOptionsPage(parent), dlg(0)
{}

QIcon OptionsPage::categoryIcon() const {
	return QIcon(":/Images/Radar.png");
}

QWidget * OptionsPage::createPage(QWidget *parent) {
	dlg = new TeamRadarDlg(parent);
	return dlg;
}

void OptionsPage::apply() {
	dlg->save();
}

///////////////////////////////////////////////////////////////////////
AboutOptionPage::AboutOptionPage(QObject* parent) : OptionsPage(parent) {}

QWidget *AboutOptionPage::createPage(QWidget* parent)
{
	QWidget* widget = new QWidget(parent);
	QLabel* label = new QLabel(tr(
		"<H1 align=\"center\">Team Radar</H1>"
		"<P align=\"center\">A continuous awareness plugin</P>"
		"<P align=\"center\">Cong Chen &lt;CongChenUTD@Gmail.com&gt;</P>"
		"<P align=\"center\">Built on %1</P>")
		.arg(Setting::getInstance()->getCompileDate()), widget);
	QHBoxLayout* layout = new QHBoxLayout(widget);
	layout->addWidget(label);
	return widget;
}

} // namespace TeamRadar
