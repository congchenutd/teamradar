#include "Analyzer.h"

namespace TeamRadar {

Analyzer::Analyzer(QStandardItemModel* m, QWidget* parent) :
	QDialog(parent), model(m)
{
	ui.setupUi(this);
}

} // namespace TeamRadar
