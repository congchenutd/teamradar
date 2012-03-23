#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>

namespace TeamRadar {

class ImageLabel : public QLabel
{
	Q_OBJECT
public:
	ImageLabel(QWidget* parent = 0);

protected:
	void mousePressEvent(QMouseEvent*);

signals:
	void valueChanged();
};

} // namespace TeamRadar

#endif // IMAGELABEL_H
