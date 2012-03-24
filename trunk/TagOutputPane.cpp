#include "TagOutputPane.h"
#include <QListWidget>
#include <QFileInfo>

namespace TeamRadar {

TagOutputPane::TagOutputPane(QObject* parent) : IOutputPane(parent) {
	todoList = new QListWidget;
}

TagOutputPane::~TagOutputPane() {
	delete todoList;
}

void TagOutputPane::addItem(const QString& text, const QString& filePath, int row, const TagKeyword& tag)
{
	QListWidgetItem* item = new QListWidgetItem;
	item->setIcon(tag.icon);
	item->setBackgroundColor(tag.bgColor);
	item->setData(FilePathRole,   filePath);   // hide meta data here
	item->setData(LineNumberRole, row);
	item->setToolTip(filePath + ":" + QString::number(row));
	item->setText(text + " (" + QFileInfo(filePath).fileName() + ":" + QString::number(row) + ") ");
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
