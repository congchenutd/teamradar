#ifndef MessageCollector_h__
#define MessageCollector_h__

#include <QObject>
#include <coreplugin/icore.h>
#include <coreplugin/imode.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/editormanager/editormanager.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorer.h>

namespace TeamRadar {

struct TeamRadarEvent;
class VCSBaseSubmitEditor;

// Captures signals from IDE, and notifies the player and the server
class MessageCollector : public QObject
{
	Q_OBJECT

public:
	MessageCollector();
	static MessageCollector* getInstance();

private slots:
	void onCurrentFileChanged(Core::IEditor *editor);
	void onOpenFile  (Core::IEditor* editor);
	void onChangeFile();
	void onChangeMode(Core::IMode* mode, Core::IMode* oldMode);
	void onOpenProject(ProjectExplorer::Project* project);
	void onEditorAboutToClose(Core::IEditor* editor);
	void onBuild(bool success);

signals:
	void localEvent(const TeamRadarEvent& event);

private:
	void sendEvent(const QString& event, const QString& parameters);  // to the server
	void sendLocalEvent (const QString& event, const QString& parameters = QString());
	void sendRemoteEvent(const QString& event, const QString& parameters = QString());

private:
	static MessageCollector* instance;

	Core::EditorManager* editorManager;
	Core::ModeManager*   modeManager;
	Core::IEditor*       currentEditor;
	ProjectExplorer::ProjectExplorerPlugin* projectExplorer;
//	Core::FileManager*   fileManager;
};

} // namespace TeamRadar

#endif // MessageCollector_h__
