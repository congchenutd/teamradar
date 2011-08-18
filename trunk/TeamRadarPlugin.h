#ifndef TEAMRADARPLUGIN_H
#define TEAMRADARPLUGIN_H

#include <extensionsystem/iplugin.h>
#include <coreplugin/icore.h>
#include <coreplugin/inavigationwidgetfactory.h> 
#include "TeamRadarWindow.h"
#include "TeamRadarView.h"

namespace Core {
	class EditorManager;
	class IEditor;
	class IMode;
}

class TeamRadarNavigationWidgetFactory;
class TeamRadarPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    bool initialize(const QStringList &arguments, QString *error_message);
	void extensionsInitialized() {}

private slots:
    void onTeamRadarOption();

private:
	Setting* setting;
};

class TeamRadarNavigationWidgetFactory : public Core::INavigationWidgetFactory 
{ 
public: 
	Core::NavigationView createWidget(); 
	QString displayName() const;
	QString id() const;
	virtual int priority() const { return 600; }
}; 

#endif // TEAMRADARPLUGIN_H
