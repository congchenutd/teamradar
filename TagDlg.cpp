#include "TagDlg.h"

namespace TeamRadar {

TagDlg::TagDlg(QWidget* parent) : QDialog(parent) {
	ui.setupUi(this);
}

QString TagDlg::getName() {
	return ui.leKeyword->text();
}

QColor TagDlg::getColor() {
	return ui.labelColor->getColor();
}

QIcon TagDlg::getIcon() {
	return ui.listWidget->currentItem()->icon();
}

void TagDlg::setName(const QString& name){
	ui.leKeyword->setText(name);
}

void TagDlg::setColor(const QColor& color) {
	ui.labelColor->setColor(color);
}

void TagDlg::setIcon(const QString& iconType)
{
	static QStringList iconTypes(
				QStringList() << "bug" << "critical" << "information" << "todo" << "warning");
	ui.listWidget->setCurrentRow(iconTypes.indexOf(iconType.toLower()));
}

}  // namespace TeamRadar
