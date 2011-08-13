#include "MessageCollector.h"
#include "Connection.h"
#include "Setting.h"
#include <QtGui/QMessageBox>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorer.h>
#include <coreplugin/filemanager.h>

MessageCollector::MessageCollector()
{
	currentEditor = 0;
	editorManager = Core::ICore::instance()->editorManager();
	modeManager   = Core::ICore::instance()->modeManager();
//	fileManager   = Core::ICore::instance()->fileManager();
	projectExplorer = ProjectExplorer::ProjectExplorerPlugin::instance();

	connect(projectExplorer, SIGNAL(currentProjectChanged(ProjectExplorer::Project*)), this, SLOT(onOpenProject(ProjectExplorer::Project*)));
	connect(editorManager,   SIGNAL(currentEditorChanged(Core::IEditor*)),   this, SLOT(onCurrentChanged(Core::IEditor*)));
	connect(editorManager,   SIGNAL(editorCreated(Core::IEditor*, QString)), this, SLOT(onOpenFile(Core::IEditor*)));
	connect(editorManager,   SIGNAL(editorsClosed(QList<Core::IEditor*>)),   this, SLOT(onCloseFiles(QList<Core::IEditor*>)));
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
	if(currentEditor != 0) {
		sendEvent("SAVE", currentEditor->file()->fileName());
	}
}

void MessageCollector::onCloseFiles(QList<Core::IEditor*> editors) {
	foreach(Core::IEditor* editor, editors) {
		sendEvent("CLOSE", editor->file()->fileName());
	}
}

void MessageCollector::onCurrentChanged(Core::IEditor* editor) {
	currentEditor = editor;
}

void MessageCollector::onChangeMode(Core::IMode* mode, Core::IMode* oldMode)
{
	Q_UNUSED(oldMode);
	sendEvent("MODE", mode->displayName());
}

void MessageCollector::sendEvent(const QString& event, const QString& parameters)
{
	QByteArray body = QString(event + '#' + parameters).toUtf8();
	QByteArray localMessage = Setting::getInstance()->getUserName().toUtf8() + '#' + body;
	emit localEvent(localMessage);

	Sender::getInstance()->sendEvent(event, parameters);
}

void MessageCollector::onOpenProject(ProjectExplorer::Project* project) {
	if(project != 0)
		sendEvent("OPENPROJECT", project->projectDirectory());
}

MessageCollector* MessageCollector::instance = 0;