#include "ImageLabel.h"
#include <QFileDialog>

namespace TeamRadar {

ImageLabel::ImageLabel(QWidget* parent) : QLabel(parent) {}

void ImageLabel::mousePressEvent(QMouseEvent *)
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Choose Image"), ".",
													"Images (*.png *.jpg *.bmp *.ico)");
	if(!fileName.isEmpty())
	{
		setPixmap(QPixmap(fileName).scaled(128, 128));
		emit valueChanged();
	}
}

} // namespace TeamRadar
