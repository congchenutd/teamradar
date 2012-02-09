#include "Analyzer.h"

Analyzer::Analyzer(QStandardItemModel* m, QWidget* parent) :
	QDialog(parent), model(m)
{
	ui.setupUi(this);
}
