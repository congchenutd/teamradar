#include "TeamRadarView.h"
#include "Node.h"
#include "NodeLabel.h"
#include "Engine.h"
#include "LightTrail.h"
#include "TeamRadarEvent.h"
#include "Setting.h"
#include "Utility.h"
#include <QtGui>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <math.h>

namespace TeamRadar {

TeamRadarView::TeamRadarView(QWidget *parent)
: QGraphicsView(parent)
{
	currentNode = 0;
	scaleFactor = 1.0;
	picking     = false;
	dragging    = false;
	setRoot(0);

	viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
	setCacheMode(CacheBackground);
	setViewportUpdateMode(BoundingRectViewportUpdate);
	setRenderHints(QPainter::Antialiasing);
	setTransformationAnchor(AnchorUnderMouse);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy  (Qt::ScrollBarAlwaysOff);
    setBackgroundBrush(QBrush(Setting::getInstance()->getColor("BackgroundColor")));
	initActions();
	TeamRadarNode::setGraph(this);
	NodeLabel::setGraph(this);
	engine = new LayeredEngine(this);
	setAfterImageDuration(Setting::getInstance()->value("AfterImageDuration").toInt());
}

void TeamRadarView::setAfterImageDuration(int seconds)
{
	AfterimageNode   ::setFadeOutDuration(seconds * 1000);
	LightTrail       ::setFadeOutDuration(seconds * 1000);
	LightTrailSegment::setFadeOutDuration(seconds * 500);
}

void TeamRadarView::initActions()
{
	actionAddDir   = new QAction(tr("Add Dir"),  this);
	actionAddFile  = new QAction(tr("Add File"), this);
	actionDel      = new QAction(tr("Delete"),   this);
	actionExpand   = new QAction(tr("Expand"),   this);
	actionCollapse = new QAction(tr("Collapse"), this);
	actionPin      = new QAction(tr("Pin"),      this);
	actionPin->setCheckable(true);
	actionWorkOn   = new QAction(tr("Work on"),  this);
	actionChat     = new QAction(tr("Chat"),     this);
	connect(actionPin,      SIGNAL(triggered(bool)), this, SLOT(onPin(bool)));
	connect(actionAddDir,   SIGNAL(triggered()), this, SLOT(onAddDir()));
	connect(actionAddFile,  SIGNAL(triggered()), this, SLOT(onAddFile()));
	connect(actionDel,      SIGNAL(triggered()), this, SLOT(onDel()));
	connect(actionExpand,   SIGNAL(triggered()), this, SLOT(onExpand()));
	connect(actionCollapse, SIGNAL(triggered()), this, SLOT(onCollapse()));
	connect(actionWorkOn,   SIGNAL(triggered()), this, SLOT(onWorkOn()));
	connect(actionChat,     SIGNAL(triggered()), this, SLOT(onChat()));
}

void TeamRadarView::itemMoved() {
	engine->start();   // when an item is moved, start the layout engine
}

void TeamRadarView::keyPressEvent(QKeyEvent *event)
{
	switch(event->key())
	{
	case Qt::Key_Up:
        moveCanvasBy(0, 20 / scaleFactor);
		break;
	case Qt::Key_Down:
		moveCanvasBy(0, -20 / scaleFactor);
		break;
	case Qt::Key_Left:
		moveCanvasBy(20 / scaleFactor, 0);
		break;
	case Qt::Key_Right:
		moveCanvasBy(-20 / scaleFactor, 0);
		break;
	case Qt::Key_Plus:
		scaleBy(1.2);
		break;
	case Qt::Key_Minus:
		scaleBy(1 / 1.2);
		break;
	case Qt::Key_Space:
	case Qt::Key_Enter:
		if(isLoaded())
			getRoot()->randomizeChildren();
		break;
	default:
		QGraphicsView::keyPressEvent(event);
	}
}

void TeamRadarView::wheelEvent(QWheelEvent *event)
{
    scaleBy(pow((double)2, event->delta() / 300.0));
	centerTree();
}

void TeamRadarView::scaleBy(qreal scaleFactor)
{
	qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if(0.2 <= factor && factor <= 4)
	{
		scale(scaleFactor, scaleFactor);
		this->scaleFactor *= scaleFactor;
	}
}

void TeamRadarView::resetScale()
{
    QRectF unity = transform().mapRect(QRectF(0, 0, 1, 1));
    if(unity.isEmpty())
        return;
    scaleBy(qMin(1/unity.width(), 1/unity.height()));
}

void TeamRadarView::scaleTo(qreal scaleFactor)
{
    resetScale();
    scaleBy(scaleFactor);
}

void TeamRadarView::loadDir(const QString& dirPath)
{
	clear();
	if(!dirPath.isEmpty())
	{
		setRoot(createNode(true, dirPath, 0));  // create root first
        centerTree(rootNode);
	}
}

// a recursive function for expansion
void TeamRadarView::createNodesFromFS(const QString& name, TeamRadarNode* owner, int maxLevel)
{
	if(owner->getLevel() >= maxLevel)
		return;

	QFileInfoList files = QDir(toAbsolutePath(name)).entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
	foreach(QFileInfo info, files)
	{
		QString relativePath = toRelativePath(info.filePath());
		if(info.isDir() && !isFilteredDir(info.baseName()))  // valid dir, recursive
		{
			TeamRadarNode* node = createNode(true, relativePath, owner);
			createNodesFromFS(info.filePath(), node, maxLevel);
		}
		else if(!isFilteredFile(info.baseName())) {          // valid file
			createNode(false, relativePath, owner);
		}
	}
}

// a factory
TeamRadarNode* TeamRadarView::createNode(bool isDir, const QString& name, TeamRadarNode* owner)
{
	TeamRadarNode* node;
	if(isDir)
		node = new DirNode(owner, name);
	else
		node = new FileNode(owner, name);

	scene()->addItem(node);
	if(owner != 0)
		scene()->addItem(new TreeEdge(owner, node));   // add link to owner
	return node;
}

void TeamRadarView::expandNode(TeamRadarNode* node, int maxLevel)
{
	createNodesFromFS(node->getName(), node, maxLevel);
	node->randomizeChildren();

	QList<HumanNode*> humans = node->getHumans();
	foreach(HumanNode* human, humans)          // update humans' position after expansion
		human->updateOwner();
}

// save graph to xml
void TeamRadarView::save(const QString& fileName)
{
	QFile file(fileName);
	if(!file.open(QFile::WriteOnly | QFile::Truncate))
		return;
	QXmlStreamWriter xml(&file);
	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(-1);
	xml.writeStartDocument();
	xml.writeStartElement("graph");
	xml.writeAttribute("version", "1.0");
	xml.writeAttribute("scale", tr("%1").arg(scaleFactor));

	if(isLoaded())
		saveNodeToXML(xml, getRoot());
	xml.writeEndElement();
	xml.writeEndDocument();
}

// recursively save nodes
void TeamRadarView::saveNodeToXML(QXmlStreamWriter& xml, const TeamRadarNode* node)
{
	bool isDir = node->type() == DirNode::Type;
	xml.writeStartElement(isDir ? "dir" : "file");
	xml.writeAttribute("name",   node->getName());
	xml.writeAttribute("pinned", node->isPinned() ? "yes" : "no");
	xml.writeAttribute("x", tr("%1").arg((int)node->x()));
	xml.writeAttribute("y", tr("%1").arg((int)node->y()));
	if(isDir)
	{
		Nodes children = node->getChildren();
		foreach(TeamRadarNode* child, children)       // save all children
			saveNodeToXML(xml, child);
	}
	xml.writeEndElement();
}

// load graph from xml
void TeamRadarView::open(const QString &fileName)
{
	QFile file(fileName);
	if(!file.open(QFile::ReadOnly))
		return;
	QXmlStreamReader xml(&file);
	if(!xml.readNextStartElement() || xml.name() != "graph")
		return;

	// load scale
	double factor = xml.attributes().value("scale").toString().toDouble();
	if(factor > 0.0)
		scaleTo(factor);

	clear();
	if(xml.readNextStartElement())
	{
		setRoot(loadNodeFromXML(xml, 0, true));   // recursively build the tree
		getRoot()->updateSize();
		getRoot()->updateDepth();
	}
	centerTree();
}

// recursive
TeamRadarNode* TeamRadarView::loadNodeFromXML(QXmlStreamReader& xml, TeamRadarNode* parent, bool isDir)
{
	QString name = xml.attributes().value("name") .toString();
	int x        = xml.attributes().value("x")    .toString().toInt();
	int y        = xml.attributes().value("y")    .toString().toInt();
	bool pinned  = xml.attributes().value("pinned") == "yes";
	TeamRadarNode* node = createNode(isDir, name, parent);
	node->setPos(x, y);
	node->setPinned(pinned);

	while(xml.readNextStartElement())
		loadNodeFromXML(xml, node, xml.name() == "dir");
	return node;
}

void TeamRadarView::setShowLabels(bool show)
{
	foreach(QGraphicsItem* item, items())
		if(TeamRadarNode* node = castToTeamRadarNode(item))
			node->setShowLabel(show);
}

void TeamRadarView::mouseMoveEvent(QMouseEvent* event)
{
	if(dragging)      // move canvas
	{
        moveCanvasBy(event->pos() - lastMousePos);
		lastMousePos = event->pos();
	}
	QGraphicsView::mouseMoveEvent(event);
}

void TeamRadarView::mousePressEvent(QMouseEvent* event)
{
	if(event->button() == Qt::LeftButton)
	{
		if(picking)  // wait the user to select the target file a human works on
		{
			TeamRadarNode* node = castToTeamRadarNode(itemAt(event->pos()));
			picking = false;
			if(node != 0)
				currentHuman->setWorkOn(node->getName());
		}
		else   // start dragging
		{
			lastMousePos = event->pos();
			dragging = itemAt(lastMousePos) == 0;   // click on canvas?
			setCursor(Qt::ClosedHandCursor);
		}
	}
	QGraphicsView::mousePressEvent(event);
}

void TeamRadarView::mouseReleaseEvent(QMouseEvent* event)
{
	dragging = false;
	unsetCursor();
	QGraphicsView::mouseReleaseEvent(event);
}

void TeamRadarView::contextMenuEvent(QContextMenuEvent *event)
{
	// catch the node, get the menu, and show it
	currentNode = castToTeamRadarNode(itemAt(event->pos()));
	if(currentNode == 0)
		return;

	if(HumanNode* human = qobject_cast<HumanNode*>(currentNode))
		currentHuman = human;

	currentNode->getContextMenu().exec(event->globalPos());
}

void TeamRadarView::onAddDir()
{
	QString name = QInputDialog::getText(this, tr("Add Dir"), tr("Dir Name"));
	if(!name.isEmpty())
	{
		createNode(true, currentNode->getName() + "/" + name, currentNode);
		// otherwise may end up selecting multiple nodes, and drag them together
		currentNode->setSelected(false);
	}
}

void TeamRadarView::onAddFile()
{
	QString name = QInputDialog::getText(this, tr("Add File"), tr("File Name"));
	if(!name.isEmpty())
	{
		createNode(false, currentNode->getName() + "/" + name, currentNode);
		// otherwise may end up selecting multiple nodes, and drag them together
		currentNode->setSelected(false);
	}
}

void TeamRadarView::onDel()
{
	currentNode->removeChildren();      // remove children
	currentNode->suicide();             // kill itself

	// clear other items that remains (i.e. labels)
	// can not move this line to the front, because the menu is still on
	if(currentNode == getRoot())
		clear();

	currentNode = 0;
}

void TeamRadarView::onExpand()
{
	bool ok;
	int maxLevel = QInputDialog::getInteger(0, tr("Expand"), tr("Expand to Level"),
		currentNode->getLevel()+1, currentNode->getLevel()+1, TeamRadarNode::MaxLevel, 1, &ok);
	if(ok)
		expandNode(currentNode, maxLevel);
}

void TeamRadarView::onCollapse() {
	currentNode->removeChildren();
}

void TeamRadarView::onPin(bool pin) {
	currentNode->setPinned(pin);
}

void TeamRadarView::clear()
{
	scene()->clear();
	humans.clear();
	setRoot(0);
}

void TeamRadarView::addDeveloper(const QString& name, const QImage& image)
{
	if(humanExists(name))
		return;
	currentHuman = new HumanNode(name, image);
	scene()->addItem(currentHuman);
	humans.insert(name, currentHuman);
	currentHuman->enterCanvas();
}

void TeamRadarView::onWorkOn()
{
	picking = true;
	setCursor(Qt::PointingHandCursor);  // wait the user for picking the target
}

void TeamRadarView::moveDeveloperTo(const QString& name, const QString& relativePath)
{
	if(HumanNode* human = findDeveloper(name))
	{
		currentHuman = human;
		currentHuman->setWorkOn(relativePath);
	}
}

void TeamRadarView::removeDeveloper(const QString& name)
{
	Humans::Iterator it = humans.find(name);
	if(it != humans.end())
	{
		it.value()->leaveCanvas();   // do not call suicide, it's different in human
		humans.erase(it);
	}
}

// auto-scale and center the tree
void TeamRadarView::autoScale()
{
    QRectF itemsRect = scene()->itemsBoundingRect();
    if(!size().isValid() || !itemsRect.isValid())
        return;

    const int margin = 50;
    qreal xRatio = (qreal)size().width()  / (itemsRect.width()  + margin);
    qreal yRatio = (qreal)size().height() / (itemsRect.height() + margin);
    scaleTo(qMin(xRatio, yRatio));

    centerTree();
}

void TeamRadarView::centerTree(TeamRadarNode* selectedNode)
{
    if(selectedNode != 0)                    // center by the specified node
    {
        selectedNode->setSelected(true);     // otherwise may center by rect
        moveCanvasBy(sceneRect().center()-selectedNode->pos());
    }
    else	// center by selected node, or the center of the rect
    {
        TeamRadarNode* selectedNode = 0;
        QList<QGraphicsItem*> items = scene()->selectedItems();
        if(!items.isEmpty())
            if(TeamRadarNode* node = castToTeamRadarNode(items.front()))
                selectedNode = node;

        QPointF center = (selectedNode == 0) ? scene()->itemsBoundingRect().center()
                                             : selectedNode->pos();
        moveCanvasBy(sceneRect().center()-center);    // move the center of the rect to 0,0
    }
    centerOn(sceneRect().center());    // scroll the view to keep the scene centered
}

void TeamRadarView::moveCanvasBy(qreal x, qreal y)
{
    const QRectF rect = sceneRect();
    scene()->setSceneRect(rect.x() - x / scaleFactor, rect.y() - y / scaleFactor,
                          rect.width(), rect.height());
}

void TeamRadarView::moveCanvasBy(const QPointF& vec) {
    moveCanvasBy(vec.x(), vec.y());
}

TeamRadarNode* TeamRadarView::castToTeamRadarNode(QGraphicsItem* item) const
{
	if(item == 0)
		return 0;
	int type = item->type();
    return type == DirNode::Type || type == FileNode::Type || type == HumanNode::Type
			? static_cast<TeamRadarNode*>(item) : 0;
}

void TeamRadarView::setDeveloperMode(const QString& developerName, const QString& mode)
{
	Humans::Iterator it = humans.find(developerName);
	if(it != humans.end())
	{
		currentHuman = it.value();
		currentHuman->setMode(mode);
	}
}

void TeamRadarView::setEffectsEnabled(bool enable)
{
	foreach(QGraphicsItem* item, items())
		if(TeamRadarNode* node = castToTeamRadarNode(item))
			node->setEffectsEnabled(enable);
	Setting::getInstance()->setValue("UseEffects", enable);
}

void TeamRadarView::onChat()
{
	if(currentHuman != 0)
		currentHuman->chat();
}

bool TeamRadarView::humanExists(const QString& name) {
	return humans.contains(name);
}

HumanNode* TeamRadarView::findDeveloper(const QString& name)
{
	Humans::Iterator it = humans.find(name);
	return it != humans.end() ? it.value() : 0;
}

void TeamRadarView::closeEvent(QCloseEvent*)
{
//	engine->stop();
//	clear();
}

bool TeamRadarView::viewportEvent(QEvent *event)
{
	switch (event->type()) {
	case QEvent::TouchBegin:
	case QEvent::TouchUpdate:
	case QEvent::TouchEnd:
	{
		QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
		QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
		if (touchPoints.count() == 2) {
			// determine scale factor
			const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
			const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
			qreal currentScaleFactor =
					QLineF(touchPoint0.pos(), touchPoint1.pos()).length()
					/ QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
			if (touchEvent->touchPointStates() & Qt::TouchPointReleased) {
				// if one of the fingers is released, remember the current scale
				// factor so that adding another finger later will continue zooming
				// by adding new scale factor to the existing remembered value.
				scaleFactor *= currentScaleFactor;
				currentScaleFactor = 1;
			}
			setTransform(QTransform().scale(scaleFactor * currentScaleFactor,
											scaleFactor * currentScaleFactor));
		}
		return true;
	}
	default:
		break;
	}
	return QGraphicsView::viewportEvent(event);
}

void TeamRadarView::setDeveloperImage(const QString& name, const QImage& image) {
	if(HumanNode* human = findDeveloper(name))
		human->setImage(image);
}

void TeamRadarView::setDeveloperColor(const QString& name, const QColor& color) {
	if(HumanNode* human = findDeveloper(name))
		human->setColor(color);
}

void TeamRadarView::setDirty(const QString& filePath, DirtyType dirty)
{
	TeamRadarNode* node = rootNode->findDescendent(filePath);
	node->setDirty(dirty);
}

} // namespace TeamRadar
