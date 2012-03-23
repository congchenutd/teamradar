#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <math.h>
#include "Edge.h"
#include "Node.h"
#include "Setting.h"

namespace TeamRadar {

Edge::Edge(TeamRadarNode* sourceNode, TeamRadarNode* destNode, double f)
{
	setAcceptedMouseButtons(0);
	source = sourceNode;
	dest   = destNode;
	source->addEdge(this);
	dest  ->addEdge(this);
	dest->setLevel(source->getLevel() + 1);
	setZValue(-1);
	setForce(f);
	adjust();
	color = Setting::getInstance()->getColor("EdgeColor");
}

void Edge::adjust()
{
	if(source == 0 || dest == 0)
		return;

	QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
	qreal length = line.length();

	prepareGeometryChange();
	if(length > source->getWidth())
	{
		QPointF edgeOffset((line.dx() * source->getRadius()) / length,
						   (line.dy() * source->getRadius()) / length);
		sourcePoint = line.p1() + edgeOffset;
		destPoint   = line.p2() - edgeOffset;
	}
	else {
		sourcePoint = destPoint = line.p1();
	}
}

QRectF Edge::boundingRect() const
{
	if(source == 0 || dest == 0)
		return QRectF();

	const qreal extra = 0.5;   // half of the width of pen
	return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
									  destPoint.y() - sourcePoint.y()))
		.normalized()
		.adjusted(-extra, -extra, extra, extra);
}

double Edge::getForce() const
{
	return force / 8;
		/*/2/log((double)source->getSize() * (double)dest->getSize())*/;
}

//////////////////////////////////////////////////////////////////////////
TreeEdge::TreeEdge(TeamRadarNode* sourceNode, TeamRadarNode* destNode, double force)
: Edge(sourceNode, destNode, force) {}

void TreeEdge::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	if(source == 0 || dest == 0)
		return;
	painter->setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->drawLine(sourcePoint, destPoint);
}

//////////////////////////////////////////////////////////////////////////
HumanEdge::HumanEdge(TeamRadarNode *sourceNode, TeamRadarNode *destNode, double force)
: Edge(sourceNode, destNode, force) {}

// invisible
void HumanEdge::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*)
{}

} // namespace TeamRadar
