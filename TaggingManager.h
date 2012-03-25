#ifndef TAGGINGMANAGER_H
#define TAGGINGMANAGER_H

#include "Tag.h"
#include "Communicator.h"
#include <projectexplorer/project.h>
#include <coreplugin/editormanager/editormanager.h>
#include <QObject>
#include <QFuture>
#include <QSet>

class QListWidgetItem;

namespace TeamRadar {

class TagOutputPane;
struct TaggingEvent;

class TaggingManager : public QObject
{
	Q_OBJECT

	typedef QList<Tag> Tags;

public:
	TaggingManager(TagOutputPane* output, QObject* parent = 0);

private slots:
	void onGotoRow(QListWidgetItem*);
	void onCurrentEditorChanged(Core::IEditor* editor);
	void onFileChanged();
	void onProjectChanged(ProjectExplorer::Project* project);
	void onRemoteTaggingEvent(const TaggingEvent& event);

private:
	void readFile(const QString& filePath);
	QRegExp generatePattern(const TagKeyword& keyword);
	Tag     findTag(const QString& line, const QString& filePath, int row);

	static void readCurrentProject(QFutureInterface<void>& future, TaggingManager* instance);

private:
	ProjectExplorer::Project* currentProject;
	TagOutputPane* outPane;
	Communicator*  communicator;
	TagKeywords    tagKeywords;
	Tags           tags;
	bool           reading;
};

}

#endif // TAGGINGMANAGER_H
