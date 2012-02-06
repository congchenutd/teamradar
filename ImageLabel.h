#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>

class QPushButton;
class ImageLabel : public QLabel
{
	Q_OBJECT
public:
	ImageLabel(QWidget* parent = 0);

protected:
	void enterEvent(QEvent*);
	void leaveEvent(QEvent*);

private slots:
	void onSetImage();

private:
	QPushButton* button;
};


#endif // IMAGELABEL_H
