#include "ImageLabel.h"
#include <QPushButton>
#include <QFileDialog>
#include <QVBoxLayout>

ImageLabel::ImageLabel(QWidget* parent) : QLabel(parent)
{
	button = new QPushButton(tr("Set image"), this);
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(button);
	button->hide();

	connect(button, SIGNAL(clicked()), this, SLOT(onSetImage()));
	setMouseTracking(true);
}

void ImageLabel::enterEvent(QEvent *) {
	button->show();
}

void ImageLabel::leaveEvent(QEvent *) {
	button->hide();
}

void ImageLabel::onSetImage()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Choose Image"), ".",
													"Images (*.png *.jpg *.bmp *.ico)");
	if(!fileName.isEmpty())
		setPixmap(QPixmap(fileName).scaled(128, 128));
}
