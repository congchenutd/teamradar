#include "TagOutputPane.h"
#include <QListWidget>
#include <QFileInfo>

namespace TeamRadar {

TagOutputPane::TagOutputPane(QObject* parent) : IOutputPane(parent)
{
	todoList = new QListWidget;
//	QFont font = qApp->font();
//	font.setPointSize(12);
//	todoList->setFont(font);
}

TagOutputPane::~TagOutputPane() {
	delete todoList;
}

void TagOutputPane::addTag(const Tag& tag)
{
	QListWidgetItem* item = new QListWidgetItem;
	item->setIcon(tag.keyword.icon);
	item->setBackgroundColor(tag.keyword.bgColor);
	item->setData(FilePathRole,   tag.filePath);   // hide meta data here
	item->setData(LineNumberRole, tag.lineNumber);
	item->setToolTip(tag.filePath + ":" + QString::number(tag.lineNumber));
	item->setText(tag.toString());
	todoList->addItem(item);
}

// clear all
void TagOutputPane::clearContents() {
	todoList->clear();
}

// only clear items from the file
void TagOutputPane::clearContents(const QString& filePath)
{
	for(int i = 0; i < todoList->count();)
	{
		QString storedPath = todoList->item(i)->data(FilePathRole).toString();
		if(filePath.compare(storedPath, Qt::CaseInsensitive) == 0)
			todoList->takeItem(i);   // remove row
		else
			i ++;
	}
}

void TagOutputPane::visibilityChanged(bool visible) {
	todoList->setVisible(visible);
}

void TagOutputPane::setFocus() {
	todoList->setFocus();
}

bool TagOutputPane::hasFocus() const {
	return todoList->hasFocus();
}

bool TagOutputPane::canNavigate() const {
	return todoList->count() > 1;
}

bool TagOutputPane::canNext() const {
	return todoList->currentRow() < todoList->count() && todoList->count() > 1;
}

bool TagOutputPane::canPrevious() const {
	return todoList->currentRow() > 0 && todoList->count() > 1;
}

void TagOutputPane::goToNext() {
	todoList->setCurrentRow(todoList->currentRow() + 1);
}

void TagOutputPane::goToPrev() {
	todoList->setCurrentRow(todoList->currentRow() - 1);
}

void TagOutputPane::sort() {
	todoList->sortItems(Qt::AscendingOrder);
}

}  // namespace TeamRadar
