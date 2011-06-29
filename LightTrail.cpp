#include "LightTrail.h"
#include "Node.h"
#include <QPainter>
#include <QPropertyAnimation>
#include <QGraphicsScene>
#include <QGraphicsBlurEffect>
#include <QGraphicsDropShadowEffect>

int LightTrailSegment::fadeOutDuration = 2000;

LightTrailSegment::LightTrailSegment(const QColor& c, const QPointF& start, const QPointF& end)
: line(start, end), color(c)
{
	//QGraphicsBlurEffect* blur = new QGraphicsBlurEffect(this);
	//blur->setBlurRadius(5);
        //setGraphicsEffect(blur);

	//QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);
	//effect->setBlurRadius(20);
	//effect->setOffset(0);
	//effect->setColor(Qt::white);
	//setGraphicsEffect(effect);

	QPropertyAnimation *animationOpacity = new QPropertyAnimation(this, "opacity", this);
	connect(animationOpacity, SIGNAL(finished()), this, SLOT(deleteLater()));
	animationOpacity->setDuration(fadeOutDuration);
	animationOpacity->setStartValue(0.5);
	animationOpacity->setEndValue(0.0);
	animationOpacity->start();
	QPropertyAnimation *animationWidth = new QPropertyAnimation(this, "width", this);
	animationWidth->setDuration(fadeOutDuration);
	animationWidth->setStartValue(16);
	animationWidth->setEndValue(3);
	animationWidth->start();
}

QRectF LightTrailSegment::boundingRect() const {
	return QRectF(line.p1(), line.p2());
}

QPainterPath LightTrailSegment::shape() const
{
	QPainterPath result;
	result.addPolygon(QPolygonF() << line.p1() << line.p2());
	return result;
}

void LightTrailSegment::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	painter->setPen(QPen(color, width, Qt::SolidLine, Qt::RoundCap));
	painter->drawLine(line);
}

//////////////////////////////////////////////////////////////////////////
int LightTrail::fadeOutDuration = 10000;

void LightTrail::addPoint(const QPointF& point)
{
	path << point;
	if(!lastPoint.isNull())   // add segment
		human->scene()->addItem(new LightTrailSegment(color, lastPoint, point));
	lastPoint = point;
}

LightTrail::LightTrail(HumanNode* hn)
{
//	setGraphicsEffect(new QGraphicsBlurEffect(this));
	//QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);
	//effect->setBlurRadius(20);
	//effect->setOffset(0);
	//effect->setColor(Qt::white);
	//setGraphicsEffect(effect);

	human = hn;
	color = human->getColor();

	QPropertyAnimation *animation = new QPropertyAnimation(this, "opacity", this);
	connect(animation, SIGNAL(finished()), this, SLOT(onDead()));
	animation->setDuration(fadeOutDuration);
	animation->setStartValue(0.5);
	animation->setEndValue(0.0);
	animation->start();
}

QRectF LightTrail::boundingRect() const {
	return path.boundingRect();
}

QPainterPath LightTrail::shape() const
{
	QPainterPath result;
	result.addPolygon(path);
	return result;
}

void LightTrail::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	painter->setPen(QPen(color, 6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->drawPolyline(path);
}

void LightTrail::onDead()
{
	human->lightTrailDied(this);  // let human forget you, otherwise it'll keep on calling you
	delete this;
}