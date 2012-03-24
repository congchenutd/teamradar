#ifndef KEYWORDDLG_H
#define KEYWORDDLG_H

#include <QDialog>
#include "ui_TagDlg.h"

namespace TeamRadar {

class TagDlg : public QDialog
{
	Q_OBJECT

public:
	TagDlg(QWidget* parent = 0);
	QString getName();
	QColor  getColor();
	QIcon   getIcon();
	void    setName (const QString& name);
	void    setColor(const QColor&  color);
	void    setIcon (const QString& iconType);

private:
	Ui::TagDlgClass ui;
};

}  // namespace TeamRadar

#endif // KEYWORDDLG_H
