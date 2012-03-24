#include "TagOptionsDlg.h"
#include "TagDlg.h"
#include "Setting.h"

namespace TeamRadar {

TagOptionsDlg::TagOptionsDlg(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.btAdd,   SIGNAL(clicked()), this, SLOT(onAdd()));
	connect(ui.btDel,   SIGNAL(clicked()), this, SLOT(onDel()));
	connect(ui.btReset, SIGNAL(clicked()), this, SLOT(onReset()));
	setTags(Setting::getInstance()->getTags());
}

void TagOptionsDlg::addTag(const TagKeyword& keyword)
{
	QListWidgetItem* item = new QListWidgetItem(keyword.icon, keyword.name);
	item->setBackgroundColor(keyword.bgColor);
	ui.listWidgetTags->addItem(item);
}

void TagOptionsDlg::setTags(const TagKeywords& tagList)
{
	if(tagList.isEmpty())
	{
		onReset();
		return;
	}

	foreach(const TagKeyword& tag, tagList)
		addTag(tag);
}

TagKeywords TagOptionsDlg::getTags()
{
	TagKeywords result;
	for(int i = 0; i < ui.listWidgetTags->count(); ++i)
		result.append(TagKeyword(ui.listWidgetTags->item(i)->text(),
						  ui.listWidgetTags->item(i)->icon(),
						  ui.listWidgetTags->item(i)->backgroundColor()));
	return result;
}

void TagOptionsDlg::onAdd()
{
	TagDlg dlg(this);
	if(dlg.exec() == QDialog::Accepted)
	{
		addTag(TagKeyword(dlg.getName(),
				   dlg.getIcon(),
				   dlg.getColor()));
		emit settingChanged();
	}
}

void TagOptionsDlg::onDel()
{
	ui.listWidgetTags->takeItem(ui.listWidgetTags->currentRow());
	emit settingChanged();
}

void TagOptionsDlg::onReset()
{
	ui.listWidgetTags->clear();                         // clear the list
	setTags(Setting::getInstance()->getDefaultTags());  // add default tags
	emit settingChanged();
}

}  // namespace TeamRadar
