#ifndef SETTINGDLG_H
#define SETTINGDLG_H

#include <QWidget>
#include "TagKeyword.h"
#include "ui_TagOptionsDlg.h"

namespace TeamRadar {

class TagOptionsDlg : public QWidget
{
	Q_OBJECT

public:
	TagOptionsDlg(QWidget* parent = 0);

	TagKeywords getTags();
	void setTags(const TagKeywords& list);

private:
	void addTag(const TagKeyword& keyword);

signals:
	void settingChanged();

private slots:
	void onAdd();
	void onDel();
	void onReset();

private:
	Ui::TagOptionsDlgClass ui;
};

}  // namespace TeamRadar

#endif // SETTINGDLG_H
