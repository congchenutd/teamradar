#ifndef TEAMRADARPLUGIN_H
#define TEAMRADARPLUGIN_H

#include <extensionsystem/iplugin.h>
#include <coreplugin/icore.h>
#include "TeamRadarWindow.h"
#include "TeamRadarView.h"

namespace Core {
	class EditorManager;
	class IEditor;
	class IMode;
}

namespace TeamRadar {

class TeamRadarPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    bool initialize(const QStringList &arguments, QString *error_message);
	void extensionsInitialized() {}

private slots:
    void onTeamRadarOption();

private:
	UserSetting* setting;
	TeamRadarView view;
};

} // namespace TeamRadar

#endif // TEAMRADARPLUGIN_H
