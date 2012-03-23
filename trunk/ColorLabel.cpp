#include "ColorLabel.h"
#include <QColorDialog>

namespace TeamRadar {

ColorLabel::ColorLabel(QWidget* parent) : QLabel(parent) {}

void ColorLabel::setColor(const QColor& clr)
{
	if(clr.isValid())
	{
		color = clr;
		QPixmap pixmap(QSize(1000, 30));
		pixmap.fill(color);
		setPixmap(pixmap);
		emit valueChanged();
	}
}

void ColorLabel::mousePressEvent(QMouseEvent *) {
	setColor(QColorDialog::getColor(color));
}

} // namespace TeamRadar
