#include "MessageCollector.h"
#include "Connection.h"
#include "Setting.h"
#include <QtGui/QMessageBox>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorer.h>
#include <coreplugin/filemanager.h>
#include <vcsbase/vcsplugin.h>
#include <vcsbase/corelistener.h>
#include <vcsbase/vcsbasesubmiteditor.h>

MessageCollector::MessageCollector()
{
	currentEditor = 0;
	editorManager = Core::ICore::instance()->editorManager();
	modeManager   = Core::ICore::instance()->modeManager();
//	fileManager   = Core::ICore::instance()->fileManager();
	projectExplorer = ProjectExplorer::ProjectExplorerPlugin::instance();

	connect(projectExplorer, SIGNAL(currentProjectChanged(ProjectExplorer::Project*)), this, SLOT(onOpenProject(ProjectExplorer::Project*)));
	connect(editorManager,   SIGNAL(currentEditorChanged(Core::IEditor*)),   this, SLOT(onCurrentFileChanged(Core::IEditor*)));
	connect(editorManager,   SIGNAL(editorCreated(Core::IEditor*, QString)), this, SLOT(onOpenFile(Core::IEditor*)));
	connect(editorManager,   SIGNAL(editorAboutToClose(Core::IEditor*)), this, SLOT(onEditorAboutToClose(Core::IEditor*)));
	connect(modeManager,     SIGNAL(currentModeChanged(Core::IMode*, Core::IMode*)), this, SLOT(onChangeMode(Core::IMode*, Core::IMode*)));
}

MessageCollector* MessageCollector::getInstance()
{
	if(instance == 0)
		instance = new MessageCollector;
	return instance;
}

void MessageCollector::onOpenFile(Core::IEditor* editor) {
	connect(editor->file(), SIGNAL(changed()), this, SLOT(onChangeFile()));
}

void MessageCollector::onChangeFile() {
	if(currentEditor != 0)
	{
		QString filePath = currentEditor->file()->fileName();
		QString projectPath = Setting::getInstance()->value("RootPath").toString();
		filePath.remove(projectPath);
		sendEvent("SAVE", filePath.right(filePath.length() - 1));   // remove '/'
	}
}

void MessageCollector::onCurrentFileChanged(Core::IEditor* editor)
{
	// skip vcs editors, because they will save and close some tmp files
	if(qobject_cast<VCSBase::VCSBaseSubmitEditor*>(editor) == 0)
		currentEditor = editor;
}

void MessageCollector::onChangeMode(Core::IMode* mode, Core::IMode* oldMode)
{
	Q_UNUSED(oldMode);
	sendEvent("MODE", mode->displayName());
}

void MessageCollector::sendEvent(const QString& event, const QString& parameters)
{
	emit localEvent(TeamRadarEvent(Setting::getInstance()->getUserName(), event, parameters));
	Sender::getInstance()->sendEvent(event, parameters);
}

void MessageCollector::onOpenProject(ProjectExplorer::Project* project) {
	if(project != 0)
		sendEvent("OPENPROJECT", project->projectDirectory());
}

// capture version control's submit event
// FIXME: now it just captures the closing of the submission editor, 
// which does not guarantee that the submission is successfully committed
// (the user may close the editor without committing)
void MessageCollector::onEditorAboutToClose(Core::IEditor* editor)
{
	VCSBase::VCSBaseSubmitEditor* submitEditor = qobject_cast<VCSBase::VCSBaseSubmitEditor*>(editor);
	if(submitEditor == 0)
		return;
	QStringList files = submitEditor->checkedFiles();
	foreach(QString fileName, files)
		sendEvent("SCM_COMMIT", fileName);
}

MessageCollector* MessageCollector::instance = 0;