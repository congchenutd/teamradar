#include "ColorLabel.h"
#include <QColorDialog>

ColorLabel::ColorLabel(QWidget* parent) : QLabel(parent) {}

void ColorLabel::setColor(const QColor& clr)
{
	if(clr.isValid())
	{
		color = clr;
		QPixmap pixmap(QSize(1000, 30));
		pixmap.fill(color);
		setPixmap(pixmap);
	}
}

void ColorLabel::mousePressEvent(QMouseEvent *) {
	setColor(QColorDialog::getColor(color));
}
