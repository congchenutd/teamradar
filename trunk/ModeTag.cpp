#include "ModeTag.h"
#include "Node.h"
#include "Setting.h"
#include <QMessageBox>
#include <QPainter>

namespace TeamRadar {

ModeTag::ModeTag(TeamRadarNode* n) : QGraphicsObject(n), node(n)
{
	setPos(QPoint(node->getWidth()/2, -node->getWidth()/2));
	setZValue(node->zValue() + 1);
}

QRectF ModeTag::boundingRect() const {
	return QRectF(0, 0, width, width);
}

QPainterPath ModeTag::shape() const
{
	QPainterPath path;
	path.addRect(boundingRect());
	return path;
}

void ModeTag::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	QImage image = getImage();
	if(!image.isNull())
		painter->drawImage(boundingRect(), image);
}

QImage ModeTag::getImage() {
	return QImage(":/Images/" + getName() + ".png");
}

} // namespace TeamRadar
