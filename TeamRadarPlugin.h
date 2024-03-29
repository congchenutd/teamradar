#ifndef TEAMRADARPLUGIN_H
#define TEAMRADARPLUGIN_H

#include <extensionsystem/iplugin.h>
#include <coreplugin/icore.h>
#include <coreplugin/inavigationwidgetfactory.h>

namespace Core {
	class EditorManager;
	class IEditor;
	class IMode;
}

namespace TeamRadar {

class Setting;
class TeamRadarNavigationWidgetFactory;
class TeamRadarPlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT

public:
    bool initialize(const QStringList& arguments, QString* error_message);
	void extensionsInitialized() {}

private:
	Setting* setting;
};


// the Team Radar Window at bottom-left corner
class TeamRadarNavigationWidgetFactory : public Core::INavigationWidgetFactory
{
public:
	Core::NavigationView createWidget();
    QString displayName()  const { return tr("Team Radar"); }
    QString id()           const { return displayName(); }
	virtual int priority() const { return 600; }
};

} // namespace TeamRadar

#endif // TEAMRADARPLUGIN_H
