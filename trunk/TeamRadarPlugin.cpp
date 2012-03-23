#include "TeamRadarPlugin.h"
#include "TeamRadarDlg.h"
#include "MessageCollector.h"
#include "TeamRadarView.h"
#include "Connection.h"
#include "PlayerWidget.h"
#include "Setting.h"
#include "PeerManager.h"
#include "OptionsPage.h"
#include "Defines.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <coreplugin/imode.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/editormanager/editormanager.h>

#include <QtCore/QDebug>
#include <QtCore/QtPlugin>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QMainWindow>

namespace TeamRadar {

bool TeamRadarPlugin::initialize(const QStringList &arguments, QString *error_message)
{
	Q_UNUSED(arguments)
	Q_UNUSED(error_message)

	PeerModel::openDB("TeamRadar.db");
	PeerModel::createTables();

	MessageCollector::getInstance();
	PeerManager::getInstance();
	setting = MySetting<Setting>::getInstance();

	Connection* connection = Connection::getInstance(this);
	connection->setUserName(setting->getUserName());
	connection->connectToHost(setting->getServerAddress(), setting->getServerPort());

	addAutoReleasedObject(new TeamRadarNavigationWidgetFactory);
	addAutoReleasedObject(new OptionsPage);
	addAutoReleasedObject(new AboutOptionPage);

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

QString TeamRadarNavigationWidgetFactory::displayName() const {
	return "TeamRadar - " + Setting::getInstance()->getUserName();
}

QString TeamRadarNavigationWidgetFactory::id() const {
	return displayName();
}

} // namespace TeamRadar

Q_EXPORT_PLUGIN(TeamRadar::TeamRadarPlugin)
