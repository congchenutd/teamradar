#ifndef COLORLABEL_H
#define COLORLABEL_H

#include <QLabel>
#include <QColor>

class ColorLabel : public QLabel
{
	Q_OBJECT
public:
	ColorLabel(QWidget* parent = 0);
	QColor getColor() const { return color; }
	void   setColor(const QColor& clr);

protected:
	void mousePressEvent(QMouseEvent*);

signals:
	void valueChanged();

private:
	QColor color;
};

#endif // COLORLABEL_H
