#ifndef ANALYZER_H
#define ANALYZER_H

#include "ui_Analyzer.h"

class QStandardItemModel;

namespace TeamRadar {

class Analyzer : public QDialog
{
	Q_OBJECT

public:
	explicit Analyzer(QStandardItemModel* m, QWidget* parent = 0);

private:
	Ui::Analyzer ui;

	QStandardItemModel* model;
};

} // namespace TeamRadar

#endif // ANALYZER_H
