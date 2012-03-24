#include "TaggingManager.h"
#include "TagOutputPane.h"
#include <coreplugin/icore.h>
#include <coreplugin/progressmanager/progressmanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <projectexplorer/projectexplorer.h>
#include <qtconcurrent/runextensions.h>
#include <QtConcurrentRun>

namespace TeamRadar {

TaggingManager::TaggingManager(TagOutputPane* output, QObject* parent) : QObject(parent)
{
	outPane = output;
	reading = false;
	communicator = new Communicator(this);
	tagKeywords = Setting::getInstance()->getTags();

	connect(outPane->getTodoList(), SIGNAL(itemClicked(QListWidgetItem*)),
			this, SLOT(onGotoRow(QListWidgetItem*)));
	connect(outPane->getTodoList(), SIGNAL(itemActivated(QListWidgetItem*)),
			this, SLOT(onGotoRow(QListWidgetItem*)));
	connect(Core::EditorManager::instance(), SIGNAL(currentEditorChanged(Core::IEditor*)),
			this, SLOT(onCurrentEditorChanged(Core::IEditor*)));
	connect(ProjectExplorer::ProjectExplorerPlugin::instance(),
			SIGNAL(currentProjectChanged(ProjectExplorer::Project*)),
			this, SLOT(onProjectChanged(ProjectExplorer::Project*)));
}

void TaggingManager::onGotoRow(QListWidgetItem* item)
{
	QString filePath = item->data(TagOutputPane::FilePathRole).toString();
	if(QFileInfo(filePath).exists())
	{
		Core::IEditor* editor = Core::EditorManager::instance()->openEditor(filePath);
		int row = item->data(TagOutputPane::LineNumberRole).toInt();
		editor->gotoLine(row);
	}
}

// open a new file
void TaggingManager::onCurrentEditorChanged(Core::IEditor *editor)
{
	if(reading || editor == 0)
		return;

	connect(editor->file(), SIGNAL(changed()), this, SLOT(onFileChanged()));
}

// file saved
void TaggingManager::onFileChanged()
{
	if(Core::IFile* file = dynamic_cast<Core::IFile*>(sender()))
	{
		outPane->clearContents(file->fileName());
		readFile(file->fileName());
	}
}

// open a new project
void TaggingManager::onProjectChanged(ProjectExplorer::Project* project)
{
	if(project == 0 || reading)
		return;

	currentProject = project;
	outPane->clearContents();

	reading = true;
	QFuture<void> result = QtConcurrent::run(&TaggingManager::readCurrentProject, this);
	Core::ICore::instance()->progressManager()->addTask(result, tr("TodoScan"), "Todo.Plugin.Scanning");
}

void TaggingManager::onRemoteTaggingEvent(const TaggingEvent& event)
{
}

// scan a file
void TaggingManager::readFile(const QString& filePath)
{
	QFile file(filePath);
	if(!file.open(QFile::ReadOnly | QFile::Text))
		return;

	for(int lineNumber = 1; !file.atEnd(); ++lineNumber)
	{
		QString line = file.readLine();
		TagKeyword tagKeyword = findMatchingTag(line);
		if(tagKeyword.isValid())
		{
			formatLine(line, tagKeyword);
			outPane->addItem(line, filePath, lineNumber, tagKeyword);
			communicator->sendTaggingEvent(line, filePath, lineNumber);
			if(!reading)
				outPane->sort();
		}
	}
}

// scan a project
void TaggingManager::readCurrentProject(QFutureInterface<void>& future, TaggingManager *instance)
{
	QStringList filesList = instance->currentProject->files(
								ProjectExplorer::Project::ExcludeGeneratedFiles);
	future.setProgressRange(0, filesList.count()-1);
	for(int i = 0; i < filesList.count(); ++i)
	{
		instance->readFile(filesList.at(i));
		future.setProgressValue(i);
	}

	instance->outPane->sort();
	instance->reading = false;
	future.reportFinished();
}

QRegExp TaggingManager::generatePattern(const TagKeyword& tag) {
	return QRegExp("//\\s*" + tag.name + "(:|\\s)", Qt::CaseInsensitive);
}

TagKeyword TaggingManager::findMatchingTag(const QString& line)
{
	foreach(const TagKeyword& tagKeyword, tagKeywords)
		if(line.contains(generatePattern(tagKeyword)))
			return tagKeyword;
	return TagKeyword();
}

void TaggingManager::formatLine(QString& line, const TagKeyword& tag)
{
	line.replace("\n", "");
	line.replace("\r", "");
	line.replace(generatePattern(tag), tag.name + ": ");
	line = line.trimmed();
	line = QTextCodec::codecForLocale()->toUnicode(line.toAscii());
}

}
