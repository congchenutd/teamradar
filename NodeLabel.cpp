#include "NodeLabel.h"
#include "Node.h"
#include <QFileInfo>
#include <QFontMetrics>
#include <QApplication>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsBlurEffect>
#include <QTime>
#include <QFont>
#include "TeamRadarView.h"
#include "Setting.h"
#include "Utility.h"

namespace TeamRadar {

NodeLabel::NodeLabel(TeamRadarNode *n) : node(n)
{
	font.setPointSize(Setting::getInstance()->getFontSize());
	text = QFileInfo(node->getName()).fileName();
	hide();
	setEffectsEnabled(Setting::getInstance()->value("UseEffects").toBool());
}

QRect NodeLabel::textRect() const
{
	QRect rec = QFontMetrics(font).boundingRect(text);
	// UGLY: extra space to avoid incomplete text, shouldn't needed
	return QRect(-rec.width()/2-2*margin, -rec.height()-margin, rec.width()+margin*4, rec.height()+margin*3);
}

void NodeLabel::update()
{
	setZValue(TeamRadarNode::MaxLevel + node->getLevel() + 2);
	setPos(node->mapToScene(0, -node->getRadius()));  // move to the top of the node
	QGraphicsItem::update();
}

void NodeLabel::show()
{
	update();
	QGraphicsItem::show();
}

void NodeLabel::addToScene(QGraphicsScene* scene) {
	scene->addItem(this);
}

TeamRadarView* NodeLabel::graph = 0;

void NodeLabel::setGraph(TeamRadarView* g) {
	graph = g;
}

QRectF NodeLabel::boundingRect() const {
	return textRect().adjusted(-margin, -margin, margin, margin);
}

QPainterPath NodeLabel::shape() const
{
	QPainterPath path;
	path.addRect(boundingRect());
	return path;
}

void NodeLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	update();                           // recalculate pos if scaled
	setScale(1.0 / graph->getScale());  // keep the scale

	painter->setFont(font);
//	painter->setPen(QPen(Setting::getInstance()->getColor("LabelTextColor")));
	painter->setPen(QPen(node->getColor()));
	painter->drawText(textRect(), text);
}

void NodeLabel::setEffectsEnabled(bool enable)
{
	if(enable)
	{
		QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);
		effect->setBlurRadius(10);
		effect->setOffset(0);
		effect->setColor(complementColor(Setting::getInstance()->getColor("BackgroundColor")));
		setGraphicsEffect(effect);
	}
	else
		setGraphicsEffect(0);
}

} // namespace TeamRadar
