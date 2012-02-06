#ifndef ANALYZER_H
#define ANALYZER_H

#include "ui_Analyzer.h"

class Analyzer : public QDialog
{
	Q_OBJECT
	
public:
	explicit Analyzer(QWidget *parent = 0);
	
private:
	Ui::Analyzer ui;
};

#endif // ANALYZER_H
