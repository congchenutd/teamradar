#include "TeamRadarPlugin.h"
#include "TeamRadarDlg.h"
#include "MessageCollector.h"
#include "TeamRadarView.h"
#include "Connection.h"
#include "PlayerWidget.h"
#include "Setting.h"
#include "PeerManager.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <coreplugin/imode.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/editormanager/editormanager.h>

#include <QtCore/QDebug>
#include <QtCore/QtPlugin>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QMainWindow>


bool TeamRadarPlugin::initialize(const QStringList &arguments, QString *error_message)
{
    Q_UNUSED(arguments)
    Q_UNUSED(error_message)

    // Get the primary access point to the workbench.
    Core::ICore *core = Core::ICore::instance();

    // Create a unique context id for our own view, that will be used for the
    // menu entry later.
    //QList<int> context = QList<int>() << Core::Constants::C_GLOBAL_ID;
	const Core::Context globalContext(Core::Constants::C_GLOBAL);

    // Create an action to be triggered by a menu entry
    QAction *actionTeamRadar = new QAction(tr("TeamRadar"), this);
    connect(actionTeamRadar, SIGNAL(triggered()), SLOT(onTeamRadarOption()));

    // Register the action with the action manager
    Core::ActionManager *actionManager = core->actionManager();
    Core::Command *command =
            actionManager->registerAction(
                    actionTeamRadar, "TeamRadar.TeamRadarOption", globalContext);

    // Request the Tools menu and add the menu to it
    Core::ActionContainer *toolsMenu =
            actionManager->actionContainer(Core::Constants::M_TOOLS);
	toolsMenu->addAction(command);

	// My stuff

	PeerModel::openDB("TeamRadar.db");
	PeerModel::createTables();

	MessageCollector::getInstance();
	PeerManager::getInstance();
	setting = MySetting<Setting>::getInstance();
	Connection* connection = Connection::getInstance(this);
	connection->setUserName(setting->getUserName());
	connection->connectToHost(setting->getServerAddress(), setting->getServerPort());

	// End my stuff

	addAutoReleasedObject(new TeamRadarNavigationWidgetFactory); 

    return true;
}

void TeamRadarPlugin::onTeamRadarOption()
{
    TeamRadarDlg dlg(0);
	dlg.exec();
}

Q_EXPORT_PLUGIN(TeamRadarPlugin)


//////////////////////////////////////////////////////////////////////////
// Factory
Core::NavigationView TeamRadarNavigationWidgetFactory::createWidget()
{
	Core::NavigationView view; 
	view.widget = new PlayerWidget;
	return view;
}

QString TeamRadarNavigationWidgetFactory::displayName() const {
	return "TeamRadar - " + Setting::getInstance()->getUserName();
}

QString TeamRadarNavigationWidgetFactory::id() const {
	return displayName();
}
