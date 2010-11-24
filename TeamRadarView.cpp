#include "TeamRadarView.h"
#include "Connection.h"
#include <QDebug>
#include <QMessageBox>

using namespace TeamRadar;

TeamRadar::TeamRadarView::TeamRadarView(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);

	ui.splitter->setSizes(QList<int>() << height() * 0 << height() * 1);
	connect(Connection::getInstance(), SIGNAL(newMessage(QString)), this, SLOT(onNewMessage(QString)));
}

void TeamRadar::TeamRadarView::onNewMessage(const QString& message) {
	ui.teEvents->appendPlainText(message);
}


//////////////////////////////////////////////////////////////////////////
// Factory
Core::NavigationView TeamRadar::TeamRadarNavigationWidgetFactory::createWidget()
{
	Core::NavigationView view; 
	view.widget = new TeamRadarView;
	return view;
}

QString TeamRadar::TeamRadarNavigationWidgetFactory::displayName() const {
	return "TeamRadar";
}

QString TeamRadar::TeamRadarNavigationWidgetFactory::id() const {
	return displayName();
}
