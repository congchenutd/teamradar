#include "TaggingManager.h"
#include "TagOutputPane.h"
#include "Utility.h"
#include <coreplugin/icore.h>
#include <coreplugin/progressmanager/progressmanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <projectexplorer/projectexplorer.h>
#include <qtconcurrent/runextensions.h>
#include <QtConcurrentRun>
#include <QTextCodec>

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
	connect(communicator, SIGNAL(remoteTagging(TaggingEvent)),
			this, SLOT(onRemoteTaggingEvent(TaggingEvent)));
}

void TaggingManager::onGotoRow(QListWidgetItem* item)
{
	QString filePath = toAbsolutePath(item->data(TagOutputPane::FilePathRole).toString());
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
		outPane->clearContents(toRelativePath(file->fileName()));
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
	tags.clear();

	reading = true;
	QFuture<void> result = QtConcurrent::run(&TaggingManager::readCurrentProject, this);
	Core::ICore::instance()->progressManager()->addTask(result, tr("TodoScan"), "Todo.Plugin.Scanning");
}

void TaggingManager::onRemoteTaggingEvent(const TaggingEvent& event) {
	outPane->addTag(event.toTag());
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

// scan a file
void TaggingManager::readFile(const QString& filePath)
{
	QFile file(filePath);
	if(!file.open(QFile::ReadOnly | QFile::Text))
		return;

	for(int lineNumber = 1; !file.atEnd(); ++lineNumber)
	{
		QString line = file.readLine();
		Tag tag = findTag(line, filePath, lineNumber);
		if(tag.isValid())
		{
			if(!tags.contains(tag))
			{
				communicator->sendTaggingEvent(tag);
				tags << tag;
			}
			outPane->addTag(tag);
			if(!reading)
				outPane->sort();
		}
	}
}

Tag TaggingManager::findTag(const QString& line, const QString& filePath, int row)
{
	Tag tag;
	foreach(const TagKeyword& keyword, tagKeywords)
		if(line.contains(generatePattern(keyword)))
		{
			QString text = line;
			text.remove("\n");
			text.remove("\r");
			text.remove(generatePattern(keyword));
			text = text.trimmed();
			text = QTextCodec::codecForLocale()->toUnicode(text.toAscii());
			tag.userName   = Setting::getInstance()->getUserName();
			tag.keyword    = keyword;
			tag.text       = text;
			tag.filePath   = toRelativePath(filePath);
			tag.lineNumber = row;
			return tag;
		}
	return tag;
}

QRegExp TaggingManager::generatePattern(const TagKeyword& keyword) {
	return QRegExp("//\\s*" + keyword.name + "(:|\\s)", Qt::CaseInsensitive);
}

}
