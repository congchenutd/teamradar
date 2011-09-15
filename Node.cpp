#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QMenu>
#include <QFileInfo>
#include <QDir>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsWidget>
#include <math.h>
#include "Edge.h"
#include "Node.h"
#include "TeamRadarView.h"
#include "Label.h"
#include "LightTrail.h"
#include "Engine.h"
#include "Setting.h"
#include "ModeTag.h"
#include "Utility.h"
#include "PeerManager.h"
#include "ChatWindow.h"

TeamRadarView* TeamRadarNode::view = 0;
qreal TeamRadarNode::sensitivity = 0.1;

TeamRadarNode::TeamRadarNode(TeamRadarNode* owner, const QString& nm, int level) : name(nm)
{
	name.replace("\\", "/");
	this->owner = owner;
	this->level = level;
	depth  = 1;
	pinned = false;
	size   = 1;
	label  = new Label(this);

	setAcceptHoverEvents(true);
	setZValue(MaxLevel - getLevel());
	setFlag(ItemIsMovable);
	setFlag(ItemIsSelectable);
	setFlag(ItemSendsGeometryChanges);
	setCacheMode(DeviceCoordinateCache);
	setEffectsEnabled(Setting::getInstance()->value("UseEffects").toBool());
}

void TeamRadarNode::addEdge(Edge* edge)
{
	edges << edge;
	edge->adjust();

	// root needs to update the size and depth if one node adds a new child
	if(edge->getSource() == this && getRoot() != 0)
	{
		getRoot()->updateSize();
		getRoot()->updateDepth();
	}
}

void TeamRadarNode::removeEdge(Edge* edge)
{
	edges.removeOne(edge);

	// root needs to update the size and depth if one node removes a child
	if(edge->getSource() == this && getRoot() != 0)
	{
		getRoot()->updateSize();
		getRoot()->updateDepth();
	}
}

double TeamRadarNode::getForce() const {
	return getSize();
}

// return if moved
bool TeamRadarNode::advance()
{
	if(newPos == pos())
		return false;
	setPos(newPos);       // apply buffered position
	return true;
}

QRectF TeamRadarNode::boundingRect() const {
	return QRectF(-getRadius(), -getRadius(), getWidth(), getWidth());
}

QPainterPath TeamRadarNode::shape() const
{
	QPainterPath path;
	path.addEllipse(boundingRect());
	return path;
}

void TeamRadarNode::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	// selection rect
	drawSelectionRect(painter);

	// node
	QRadialGradient gradient(-5, -5, getRadius()*1.5);
	gradient.setColorAt(0, QColor(Qt::white));
	gradient.setColorAt(0.3, color.lighter(150));
	gradient.setColorAt(0.7, color);
	gradient.setColorAt(1, color.darker(120));
	painter->setBrush(gradient);
        painter->setPen(QPen(color.darker()));
	painter->drawEllipse(boundingRect());

	// for debugging
	//QFont font;
	//font.setPointSize(6);
	//painter->setFont(font);
	//painter->drawText(-getRadius(), getRadius()/2, tr("%1").arg(getSize()));
}

QVariant TeamRadarNode::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if(change == ItemPositionHasChanged)
	{
		foreach(Edge* edge, edges)   // update edges' position
			edge->adjust();
		view->itemMoved();           // ask view to run the engine
		label->update();
	}
	return QGraphicsItem::itemChange(change, value);
}

void TeamRadarNode::setShowLabel(bool show)
{
	if(show)
		showLabel();
	else
		hideLabel();
	setAcceptsHoverEvents(!show);
}

// show the labels of the nodes along the path to root
// do not back trace to the root
void TeamRadarNode::showLabel()
{
	label->addToScene(scene());  // FIXME: may be already added
	label->show();               // adjust pos, font
	if(getOwner() != 0 && getOwner()->getOwner() != 0)  // stops before root
		getOwner()->showLabel();
}

// hide the labels of the nodes along the path to root
void TeamRadarNode::hideLabel()
{
	label->hide();
	if(getOwner() != 0)
		getOwner()->hideLabel();
}

void TeamRadarNode::hoverEnterEvent(QGraphicsSceneHoverEvent*) {
	showLabel();
}
void TeamRadarNode::hoverLeaveEvent(QGraphicsSceneHoverEvent*) {
	hideLabel();
}

// recalculate the size of the subtree, and buffer it
void TeamRadarNode::updateSize()
{
	size = getNetSize();
	Nodes children = getChildren();
	foreach(TeamRadarNode* child, children)
	{
		child->updateSize();
		size += child->getSize();
	}
}

// find direct child by name
TeamRadarNode* TeamRadarNode::findChild(const QString& name) const
{
	Nodes children = getChildren();
	foreach(TeamRadarNode* child, children)
		if(child->getName() == name)
			return child;
	return 0;
}

// used for collapsing
void TeamRadarNode::removeChildren()
{
	Nodes children = getChildren();
	foreach(TeamRadarNode* child, children)
	{
		child->removeEdgeToOwner();
		child->removeChildren();   // recursively kill children
		child->suicide();          // kill itself (human will update owner instead)
	}
	view->itemMoved();
}

void TeamRadarNode::suicide()
{
	removeEdgeToOwner();
	hideLabel();
	scene()->removeItem(this);
	delete this;
}

// "picking and releasing the apple"
void TeamRadarNode::loosenEdgeToOwner()
{
	if(Edge* edge = findOwnerEdge())
	{
		edge->setForce(0.5);
		view->itemMoved();
	}
}
void TeamRadarNode::fastenEdgeToOwner()
{
	if(Edge* edge = findOwnerEdge())
	{
		edge->setForce(1.0);
		view->itemMoved();
	}
}

Edge* TeamRadarNode::findOwnerEdge() const
{
	foreach(Edge* edge, edges)
		if(edge->getSource() == owner)
			return edge;
	return 0;
}

Nodes TeamRadarNode::getAncestors() const
{
	Nodes result;
	TeamRadarNode* parent = getOwner();
	while(parent != 0)
	{
		result << parent;
		parent = parent->getOwner();
	}
	return result;
}

Nodes TeamRadarNode::getPushers() const
{
	Nodes pushers;
	if(getOwner() != 0)   // siblings excluding human
	{
		Nodes children = getOwner()->getChildren();
		foreach(TeamRadarNode* child, children)
			if(dynamic_cast<HumanNode*>(child) == 0 && child != this)
				pushers << child;
	}
	pushers << getAncestors();
	return pushers;
}

void TeamRadarNode::randomize()
{
//	setPos(100, 100);
	setPos(x() + qrand()%100 - 50, y() + qrand()%100 - 50);
	setPinned(false);
}

TeamRadarNode* TeamRadarNode::getRoot() const {
	return view->getRoot();
}

Nodes TeamRadarNode::getChildren() const
{
	Nodes result;
	foreach(Edge* edge, edges)
	{
		TeamRadarNode* child = edge->getDest();
		if(child != this)
			result << child;
	}
	return result;
}

// after this, the node has no relationship with the owner
void TeamRadarNode::removeEdgeToOwner()
{
	if(getOwner() == 0)
		return;
	Edge* ownerEdge = findOwnerEdge();
	if(ownerEdge != 0)
	{
		getOwner()->removeEdge(ownerEdge);
		removeEdge(ownerEdge);
		scene()->removeItem(ownerEdge);
		delete ownerEdge;
	}
}

QFileInfo TeamRadarNode::findMatchingPath(const QString& filePath)
{
	QFileInfo result;
	QFileInfoList files = QDir(getName()).entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
	foreach(QFileInfo info, files)            // for all files/dirs in this dir
	{
		QString path = info.filePath();       // find the max matching one
		if(filePath.contains(path, Qt::CaseInsensitive)
		   && path.length() > result.filePath().length())
			result = info;
	}
	return result;
}

void TeamRadarNode::setEffectsEnabled(bool enable)
{
	if(enable)
	{
		QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);
		effect->setBlurRadius(20);
		effect->setOffset(0);
		effect->setColor(complementColor(Setting::getInstance()->getColor("BackgroundColor")));
		setGraphicsEffect(effect);
	}
	else
		setGraphicsEffect(0);
	label->setEffectsEnabled(enable);
}

void TeamRadarNode::detectConflict()
{
	QList<HumanNode*> humans = getHumans();
	foreach(HumanNode* human, humans)
		human->showConflict(humans.size() > 1);
}

QList<HumanNode*> TeamRadarNode::getHumans() const
{
	QList<HumanNode*> humans;
	Nodes children = getChildren();
	foreach(TeamRadarNode* child, children)
		if(child->type() == HumanNode::Type)
			humans << dynamic_cast<HumanNode*>(child);
	return humans;
}

void TeamRadarNode::drawSelectionRect(QPainter* painter)
{
	if(isSelected())
	{
		painter->setPen(QPen(Qt::red, 0, Qt::DashLine));
		painter->setBrush(Qt::NoBrush);
		painter->drawRect(boundingRect());
	}
}

//////////////////////////////////////////////////////////////////////////
DirNode::DirNode(TeamRadarNode* owner, const QString& name)
: TeamRadarNode(owner, name) {
	setColor(Setting::getInstance()->getColor("DefaultDirColor"));
}

void DirNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent*)
{
	if(expandable())
		view->expandNode(this, getLevel() + 1);
	else
		removeChildren();  // collapse
}

// count all non-human children
int DirNode::directChildrenCount() const
{
	int result = 0;
	Nodes children = getChildren();
	foreach(TeamRadarNode* child, children)
		if(dynamic_cast<HumanNode*>(child) == 0)
			result ++;
	return result;
}

QMenu& DirNode::getContextMenu() const
{
	static QMenu menu;
	
	// no effect if already added, may move to an init function
	menu.addAction(view->actionAddDir);
	menu.addAction(view->actionAddFile);
	menu.addAction(view->actionExpand);
	menu.addAction(view->actionCollapse);
	menu.addAction(view->actionDel);
	menu.addAction(view->actionPin);
	view->actionExpand  ->setEnabled(expandable());
	view->actionCollapse->setEnabled(collapsable());
	view->actionPin     ->setChecked(isPinned());
	return menu;
}

void DirNode::randomizeChildren()
{
	Nodes children = getChildren();
	foreach(TeamRadarNode* child, children)
	{
		child->randomize();
		child->randomizeChildren();
	}
}

void DirNode::updateDepth()
{
	depth = 1;
	Nodes children = getChildren();
	foreach(TeamRadarNode* child, children)
	{
		child->updateDepth();
		depth = qMax(depth, child->getDepth()+1);
	}
}

//////////////////////////////////////////////////////////////////////////
FileNode::FileNode(TeamRadarNode* owner, const QString& name)
: TeamRadarNode(owner, name) {
	setColor(Setting::getInstance()->getExtensionColor(name));
}

QMenu & FileNode::getContextMenu() const
{
	static QMenu menu;
	menu.addAction(view->actionDel);
	menu.addAction(view->actionPin);
	view->actionPin->setChecked(isPinned());
	return menu;
}

//////////////////////////////////////////////////////////////////////
HumanNode::HumanNode(const QString& name, const QImage& img, TeamRadarNode* owner)
: TeamRadarNode(owner, name), image(img)
{
	setFlag(ItemIsMovable, false);
	setColor(PeerManager::getInstance()->getDeveloperColor(name));
	lightTrail = 0;
	modeTag = new NullTag(this);
}

QRectF HumanNode::boundingRect() const {
	return QRect(-getRadius(), -getRadius(), getWidth(), getWidth());
}

QPainterPath HumanNode::shape() const
{
	QPainterPath path;
	path.addRect(boundingRect());
	return path;
}

void HumanNode::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
//	setScale(1.0 / view->getScale());    // keep the scale
	painter->drawImage(boundingRect(), image);
	drawSelectionRect(painter);
}

QMenu& HumanNode::getContextMenu() const
{
	static QMenu menu;
	menu.addAction(view->actionWorkOn);
	menu.addAction(view->actionDel);
	menu.addAction(view->actionChat);
	view->actionChat->setEnabled(
		Setting::getInstance()->getUserName() != getName());  // not myself
	return menu;
}

// move to a new owner
// expand the target node if expandable == true
void HumanNode::updateOwner(bool expandable)
{
	detachFromOwner();                                       // leave the original owner
	owner = findOwner(view->getRoot(), workOn, expandable);  // find a new owner
	if(owner != 0)                                           // connect to the new owner
	{
		scene()->addItem(new HumanEdge(owner, this));
		owner->loosenEdgeToOwner();
		owner->detectConflict();
		showLabel();
		view->itemMoved();
	}
}

// starting from "start", find the node it works on
// expand the node if expandable is true
TeamRadarNode* HumanNode::findOwner(TeamRadarNode* parent, const QString& path, bool expandable)
{
	QString firstSection = getFirstSection(path);
	TeamRadarNode* child = parent->findChild(firstSection);
	if(child == 0)    // not expanded
	{
		if(!expandable)
			return parent;

		child = view->createNode(firstSection, parent);
		child->randomize();
	}
	
	if(firstSection == path)   // last section
		return child;
	QString rest = path;
	rest.remove(firstSection + '/');
	return findOwner(child, rest, expandable);
}

Nodes HumanNode::getPushers() const
{
	// all other humans and ancestors
	Nodes pushers;
	foreach(HumanNode* human, view->getHumans())
		pushers << human;

	pushers << getAncestors();
	return pushers;
}

void HumanNode::lightTrailDied(LightTrail* trail) {
	if(trail == lightTrail)
		lightTrail = 0;   // otherwise advance() will continue using it
}

void HumanNode::setWorkOn(const QString& filePath)
{
	leaveAfterimage();
	
	workOn = filePath;  // find new owner
	updateOwner();
	
	lightTrail = new LightTrail(this);   // start a new light trail
	scene()->addItem(lightTrail);
}

void HumanNode::leaveAfterimage()
{
	AfterimageNode* afterimage = new AfterimageNode(getName() + "'s afterimage", image, owner);
	scene()->addItem(afterimage);
	if(owner != 0)
		scene()->addItem(new HumanEdge(owner, afterimage, 3));
	afterimage->setPos(pos());
}

void HumanNode::enterCanvas()
{
	showLabel();

	QPropertyAnimation* opacityAnimation = new QPropertyAnimation(this, "opacity", this);
	opacityAnimation->setDuration(2000);
	opacityAnimation->setStartValue(0.0);
	opacityAnimation->setEndValue(1.0);
	opacityAnimation->start();

	double theta = (qrand() % 20) * 0.314;   // randomize the angle
	QPropertyAnimation* posAnimation = new QPropertyAnimation(this, "pos", this);
	posAnimation->setDuration(2000);
	posAnimation->setStartValue(generateHeavenPos(1000, theta));
	posAnimation->setEndValue  (generateHeavenPos(scene()->width() / 2, theta));
	posAnimation->start();
}

void HumanNode::leaveCanvas()
{
	detachFromOwner();

	QPropertyAnimation* opacityAnimation = new QPropertyAnimation(this, "opacity", this);
	connect(opacityAnimation, SIGNAL(finished()), this, SLOT(onDead()));
	opacityAnimation->setDuration(2000);
	opacityAnimation->setStartValue(1.0);
	opacityAnimation->setEndValue(0.0);
	opacityAnimation->start();

	QPropertyAnimation* posAnimation = new QPropertyAnimation(this, "pos", this);
	posAnimation->setDuration(2000);
	posAnimation->setStartValue(pos());
	posAnimation->setEndValue(generateHeavenPos());
	posAnimation->start();
}

void HumanNode::detachFromOwner()
{
	if(getOwner() != 0)
	{
		getOwner()->fastenEdgeToOwner();
		removeEdgeToOwner();
		getOwner()->detectConflict();
		hideLabel();
		view->itemMoved();
	}
}

void HumanNode::onDead()
{
	TeamRadarNode::hideLabel();
	TeamRadarNode::suicide();
}

void HumanNode::setNewPos(const QPointF &p)
{
	if(isSelected())                  // camera view
		view->centerTree(this);
	if(lightTrail != 0)
		lightTrail->addPoint(pos());  // leave light trail segments
	TeamRadarNode::setNewPos(p);
}

QVariant HumanNode::itemChange(GraphicsItemChange change, const QVariant& value)
{
	// resize the view when the human loses selection
	if(change == ItemSelectedHasChanged && Engine::getEngine()->isRunning() && !value.toBool())
		view->autoScale();
	return TeamRadarNode::itemChange(change, value);
}

void HumanNode::setMode(const QString& mode)
{
	scene()->removeItem(modeTag);
	delete modeTag;
	
	if(mode == "Welcome")
		modeTag = new WelcomeModeTag(this);
	else if(mode == "Edit")
		modeTag = new EditModeTag(this);
	else if(mode == "Design")
		modeTag = new DesignModeTag(this);
	else if(mode == "Debug")
		modeTag = new DebugModeTag(this);
	else if(mode == "Projects")
		modeTag = new ProjectsModeTag(this);
	else if(mode == "Help")
		modeTag = new HelpModeTag(this);
	else if(mode == "Conflict")
		modeTag = new AlertModeTag(this);
	else
		modeTag = new NullTag(this);
	scene()->addItem(modeTag);
}

void HumanNode::showConflict(bool show)
{
	static QString lastMode;
	if(modeTag->getName() != "Conflict")
		lastMode = modeTag->getName();
	if(show)
		setMode("Conflict");
	else
		setMode(lastMode);
}

// does not hide the label of itself
void HumanNode::hideLabel()
{
	if(getOwner() != 0)
		getOwner()->hideLabel();
}

void HumanNode::chat(const QString& content)
{
	ChatWindow* chatWindow = ChatWindow::getChatWindow(getName());
	if(chatWindow != 0)
	{
		chatWindow->addPeerConversation(content);
		chatWindow->show();
	}
}

//////////////////////////////////////////////////////////////////////////////////
int AfterimageNode::fadeOutDuration = 10000;

AfterimageNode::AfterimageNode(const QString &name, const QImage &img, TeamRadarNode *owner)
	: HumanNode(name, img, owner)
{
//	image = image.scaled((1.0 / view->getScale()), (1.0 / view->getScale()));
	setGraphicsEffect(new QGraphicsBlurEffect(this));

	QPropertyAnimation* animation = new QPropertyAnimation(this, "opacity", this);
	connect(animation, SIGNAL(finished()), this, SLOT(onDead()));
	animation->setDuration(fadeOutDuration);
	animation->setStartValue(1.0);
	animation->setEndValue(0.0);
	animation->start();
}

void AfterimageNode::onDead()
{
	removeEdgeToOwner();
	scene()->removeItem(this);
	delete label;
	delete this;
//	view->itemMoved();
}

//void AfterimageNode::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
//{
//	setScale(1.0 / view->getScale());
//	painter->drawImage(boundingRect(), image);
//}

