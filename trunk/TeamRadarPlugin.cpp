#include "TeamRadarPlugin.h"
#include "Defines.h"
#include "MessageCollector.h"
#include "Connection.h"
#include "PlayerWidget.h"
#include "Setting.h"
#include "PeerManager.h"
#include "TeamRadarOptionsPage.h"
#include "TagOutputPane.h"
#include "TaggingManager.h"
#include "TagOptionsPage.h"
#include "Utility.h"

#include <QtCore/QtPlugin>

namespace TeamRadar {

bool TeamRadarPlugin::initialize(const QStringList& arguments, QString* error_message)
{
	Q_UNUSED(arguments)
	Q_UNUSED(error_message)

	// for TodoTag
	qRegisterMetaTypeStreamOperators<TagKeyword> ("TagKeyword");
	qRegisterMetaTypeStreamOperators<TagKeywords>("TagKeywords");

	TagOutputPane* outPane = new TagOutputPane(this);
	new TaggingManager(outPane, this);

	addAutoReleasedObject(outPane);
	addAutoReleasedObject(new TagOptionsPage(this));
	addAutoReleasedObject(new TagAboutPage(this));

	// team radar
	PeerModel::openDB("TeamRadar.db");
	PeerModel::createTables();

	MessageCollector::getInstance();
	PeerManager::getInstance();
	setting = MySetting<Setting>::getInstance();

	Connection* connection = Connection::getInstance(this);
	connection->setUserName(setting->getUserName());
	connection->connectToHost(setting->getServerAddress(), setting->getServerPort());

	addAutoReleasedObject(new TeamRadarNavigationWidgetFactory);
	addAutoReleasedObject(new TeamRadarOptionsPage);
	addAutoReleasedObject(new TeamRadarAboutPage);

	return true;
}


//////////////////////////////////////////////////////////////////////////
// Factory
Core::NavigationView TeamRadarNavigationWidgetFactory::createWidget()
{
	Core::NavigationView view;
	view.widget = PlayerWidget::getInstance();
	return view;
}

} // namespace TeamRadar

Q_EXPORT_PLUGIN(TeamRadar::TeamRadarPlugin)
