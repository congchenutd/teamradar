#ifndef LIGHTTRAIL_H
#define LIGHTTRAIL_H

#include <QGraphicsObject>
#include <QPolygon>
#include <QLine>

namespace TeamRadar {

class HumanNode;

// Two methods are combined to create a light trail

// One implementation of light trail: multiple independent segments
// A segment shrinks and fades out
// Pro: becomes thinner at the tail
// Con: segments overlap and the overlapping points look darker
class LightTrailSegment : public QGraphicsObject
{
	Q_OBJECT

public:
	LightTrailSegment(const QColor& c, const QPointF& start, const QPointF& end);
	static void setFadeOutDuration(int duration) { fadeOutDuration = duration; }

	Q_PROPERTY(int width READ getWidth WRITE setWidth)
	int getWidth() const { return width; }
	void setWidth(int w) { width = w;    }

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);
	QRectF boundingRect() const;
	QPainterPath shape() const;

private:
	static int fadeOutDuration;
	QLineF line;
	int width;
	QColor color;
};

// Use a curve to implement light trail
// Pro: smooth
// Con: fades out evenly
class LightTrail : public QGraphicsObject
{
	Q_OBJECT

public:
	LightTrail(HumanNode* hn);
	void addPoint(const QPointF& point);
	static void setFadeOutDuration(int duration) { fadeOutDuration = duration; }

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);

private slots:
	void onDead();

private:
	static int fadeOutDuration;
	QPolygonF path;
	HumanNode* human;
	QPointF lastPoint;
	QColor color;
};


} // namespace TeamRadar

#endif // LIGHTTRAIL_H
