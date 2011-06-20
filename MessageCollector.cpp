#include "MessageCollector.h"
#include "Connection.h"
#include <QtGui/QMessageBox>

using namespace TeamRadar;

////////////////////////////// MessageCollector ///////////////////////////////
TeamRadar::MessageCollector::MessageCollector()
{
	currentEditor = 0;
	Core::ICore* core = Core::ICore::instance();
	editorManager = core->editorManager();
	modeManager   = core->modeManager();
//	projectExplorer->saveModifiedFiles();
	//projectExplorer = ProjectExplorer::ProjectExplorerPlugin::instance();
	connect(editorManager, SIGNAL(currentEditorChanged(Core::IEditor*)),   this, SLOT(onCurrentChanged(Core::IEditor*)));
	connect(editorManager, SIGNAL(editorCreated(Core::IEditor*, QString)), this, SLOT(onOpenFile(Core::IEditor*)));
	connect(editorManager, SIGNAL(editorsClosed(QList<Core::IEditor*>)),   this, SLOT(onCloseFiles(QList<Core::IEditor*>)));
	connect(modeManager,   SIGNAL(currentModeChanged(Core::IMode*, Core::IMode*)), this, SLOT(onChangeMode(Core::IMode*, Core::IMode*)));
}

MessageCollector* TeamRadar::MessageCollector::getInstance()
{
	if(instance == 0)
		instance = new MessageCollector;
	return instance;
}

void TeamRadar::MessageCollector::onOpenFile(Core::IEditor* editor) {
	connect(editor->file(), SIGNAL(changed()), this, SLOT(onChangeFile()));
}

void TeamRadar::MessageCollector::onChangeFile()
{
	if(currentEditor != 0)
	{
//		QMessageBox::information(0, "msg", tr("File %1 Changed").arg(currentEditor->file()->fileName()));
		sendEvent("SAVE", currentEditor->file()->fileName());
	}
}

void TeamRadar::MessageCollector::onCloseFiles(QList<Core::IEditor*> editors)
{
	foreach(Core::IEditor* editor, editors)
	{
//		QMessageBox::information(0, "msg", tr("File %1 Closed").arg(editor->file()->fileName()));
		sendEvent("CLOSE", editor->file()->fileName());
	}
}

void TeamRadar::MessageCollector::onCurrentChanged(Core::IEditor* editor) {
	currentEditor = editor;
}

void TeamRadar::MessageCollector::onChangeMode(Core::IMode* mode, Core::IMode* oldMode)
{
	Q_UNUSED(oldMode);
//	QMessageBox::information(0, "msg", tr("%1 Mode").arg(mode->displayName()));
	sendEvent("MODE", mode->displayName());
}

void TeamRadar::MessageCollector::sendEvent(const QString& event, const QString& parameters)
{
	Connection* connection = Connection::getInstance();
	if(connection->getState() != Connection::ReadyForUse)
		return;
	QByteArray body = QString(event + '#' + parameters).toUtf8();
	QByteArray data = "EVENT#" + QByteArray::number(body.size()) + '#' + body;
	connection->write(data);
}

void TeamRadar::MessageCollector::onOpenProject(ProjectExplorer::Project* project)
{
	sendEvent("OpenProject", project->displayName());
}

MessageCollector* TeamRadar::MessageCollector::instance = 0;