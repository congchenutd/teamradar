#ifndef MessageCollector_h__
#define MessageCollector_h__

#include <QObject>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
//#include <coreplugin/basemode.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/imode.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/editormanager/editormanager.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorer.h>

class MessageCollector : public QObject
{
	Q_OBJECT

public:
	MessageCollector();
	static MessageCollector* getInstance();

private slots:
	void onCurrentChanged(Core::IEditor *editor);
	void onOpenFile  (Core::IEditor* editor);
	void onChangeFile();
	void onCloseFiles(QList<Core::IEditor*>);
	void onChangeMode(Core::IMode* mode, Core::IMode* oldMode);
	void onOpenProject(ProjectExplorer::Project* project);

signals:
	void localEvent(QString);

private:
	void sendEvent(const QString& event, const QString& parameters);

private:
	static MessageCollector* instance;

	Core::EditorManager* editorManager;
	Core::ModeManager*   modeManager;
	Core::IEditor*       currentEditor;
	ProjectExplorer::ProjectExplorerPlugin* projectExplorer;
};

#endif // MessageCollector_h__