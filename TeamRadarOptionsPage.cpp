#include "TeamRadarOptionsPage.h"
#include "TeamRadarDlg.h"
#include "Setting.h"
#include <QLabel>

namespace TeamRadar {

TeamRadarOptionsPage::TeamRadarOptionsPage(QObject* parent) : IOptionsPage(parent), dlg(0)
{}

QIcon TeamRadarOptionsPage::categoryIcon() const {
	return QIcon(":/Images/Radar.png");
}

QWidget * TeamRadarOptionsPage::createPage(QWidget *parent) {
	dlg = new TeamRadarDlg(parent);
	return dlg;
}

void TeamRadarOptionsPage::apply() {
	dlg->save();
}

///////////////////////////////////////////////////////////////////////
TeamRadarAboutPage::TeamRadarAboutPage(QObject* parent) : TeamRadarOptionsPage(parent) {}

QWidget *TeamRadarAboutPage::createPage(QWidget* parent)
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
