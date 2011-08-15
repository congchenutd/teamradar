#include "TeamRadarView.h"
#include "Node.h"
#include "Label.h"
#include "Engine.h"
#include "LightTrail.h"
#include "TeamRadarEvent.h"
#include "Setting.h"
#include "Utility.h"
#include <QtGui>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <math.h>

TeamRadarView::TeamRadarView(QWidget *parent)
: QGraphicsView(parent)
{
	currentNode = 0;
	scaleFactor = 1.0;
	picking     = false;
	dragging    = false;
	setRoot(0);

	setCacheMode(CacheBackground);
	setViewportUpdateMode(BoundingRectViewportUpdate);
	setRenderHints(QPainter::Antialiasing);
	setTransformationAnchor(AnchorUnderMouse);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setBackgroundBrush(QBrush(Setting::getInstance()->getColor("BackgroundColor")));
	initActions();
	TeamRadarNode::setGraph(this);
	Label::setGraph(this);
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
	connect(actionAddDir,   SIGNAL(triggered()), this, SLOT(onAddDir()));
	connect(actionAddFile,  SIGNAL(triggered()), this, SLOT(onAddFile()));
	connect(actionDel,      SIGNAL(triggered()), this, SLOT(onDel()));
	connect(actionExpand,   SIGNAL(triggered()), this, SLOT(onExpand()));
	connect(actionCollapse, SIGNAL(triggered()), this, SLOT(onCollapse()));
	connect(actionPin,      SIGNAL(triggered(bool)), this, SLOT(onPin(bool)));
	connect(actionWorkOn,   SIGNAL(triggered()), this, SLOT(onWorkOn()));
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
	scaleBy(pow((double)2, event->delta() / 240.0));
	centerTree();
}

void TeamRadarView::scaleBy(qreal scaleFactor)
{
	qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if(0.01 <= factor && factor <= 100)
	{
		scale(scaleFactor, scaleFactor);
		this->scaleFactor *= scaleFactor;
	}
}

void TeamRadarView::loadDir(const QString& dirPath, int maxLevel)
{
	clear();
	setRoot(createNode(true, dirPath, 0));  // create root first
	expandNode(getRoot(), maxLevel);
	centerTree();
}

// a recursive function for expansion
void TeamRadarView::createNodesFromFS(const QString& path, TeamRadarNode* owner, int maxLevel)
{
	if(owner->getLevel() >= maxLevel)
		return;
	QFileInfoList files = QDir(path).entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
	foreach(QFileInfo info, files)
		if(info.isDir() && !isFilteredDir(info.baseName()))
		{
			TeamRadarNode* node = createNode(true, info.filePath(), owner);
			createNodesFromFS(info.filePath(), node, maxLevel);
		}
		else if(!isFilteredFile(info.baseName())) {
			createNode(false, info.filePath(), owner);
		}
}

// a factory
TeamRadarNode* TeamRadarView::createNode(bool isDir, const QString& name, TeamRadarNode* owner)
{
	TeamRadarNode* node;
	if(isDir)
		node = new DirNode(owner, name);
	else
	{
		QTime time;
		time.start();
		node = new FileNode(owner, name);
		int msec = time.elapsed();
//		qDebug() << "-----------------------------------------------------" << name << ": " << msec;
	}
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
	bool isDir = dynamic_cast<const DirNode*>(node);
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

void TeamRadarView::moveCanvasBy(int x, int y)
{
	const QRectF rect = sceneRect();
	scene()->setSceneRect(rect.x() - x, rect.y() - y, rect.width(), rect.height());
}

void TeamRadarView::moveCanvasBy(const QPointF& vec) {
	moveCanvasBy(vec.x(), vec.y());
}

void TeamRadarView::setShowLabels(bool show) {
	foreach(QGraphicsItem* item, items())
		if(TeamRadarNode* node = dynamic_cast<TeamRadarNode*>(item))
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
			TeamRadarNode* node = dynamic_cast<TeamRadarNode*>(itemAt(event->pos()));
			picking = false;
			if(node != 0)
				currentHuman->setWorkOn(node->getName());
		}
		else   // start dragging
		{
			lastMousePos = event->pos();
			dragging = dynamic_cast<TeamRadarNode*>(itemAt(lastMousePos)) == 0;   // click on canvas?
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
	currentNode = dynamic_cast<TeamRadarNode*>(itemAt(event->pos()));
	if(currentNode == 0)
		return;

	currentNode->getContextMenu().exec(event->globalPos());
	if(HumanNode* human = dynamic_cast<HumanNode*>(currentNode))
		currentHuman = human;
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

void TeamRadarView::addDeveloper(const QString &name, const QString &image)
{
	if(humans.contains(name))  // already exists
		return;
	currentHuman = new HumanNode(name, QImage(image));
	scene()->addItem(currentHuman);
	humans.insert(name, currentHuman);
	currentHuman->enterCanvas();
}

void TeamRadarView::onWorkOn()
{
	picking = true;
	setCursor(Qt::PointingHandCursor);  // wait the user for picking the target
}

TeamRadarView::~TeamRadarView() {
	clear();
}

void TeamRadarView::moveDeveloperTo(const QString& name, const QString& relativePath)
{
	QString absolutePath = toAbsolutePath(getRoot()->getName(), relativePath);
	Humans::Iterator it = humans.find(name);
	if(it != humans.end())
	{
		currentHuman = it.value();
		currentHuman->setWorkOn(absolutePath);
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
	QRectF itemsRect = nodesBoundingRect();
	if(!size().isValid() || !itemsRect.isValid())
		return;

	const int margin = 50;     // don't know why it's needed
	qreal xRatio = (qreal)size().width()  / (itemsRect.width()  + margin);
	qreal yRatio = (qreal)size().height() / (itemsRect.height() + margin);
	scaleTo(qMin(xRatio, yRatio));
	centerTree();
}

void TeamRadarView::centerTree(TeamRadarNode* node)
{
	if(node != 0)
		moveCanvasBy(sceneRect().center() - node->pos());
	else	// find a selected node
	{
		TeamRadarNode* selectedNode = 0;
		QList<QGraphicsItem*> items = scene()->selectedItems();
		if(!items.isEmpty())
			if(TeamRadarNode* node = dynamic_cast<TeamRadarNode*>(items.front()))
				selectedNode = node;

		QPointF center = selectedNode == 0 ? nodesBoundingRect().center()
			: selectedNode->pos();
		moveCanvasBy(sceneRect().center() - center);
	}
}

QRectF TeamRadarView::nodesBoundingRect() const
{
	QRectF boundingRect;
	foreach(QGraphicsItem* item, items())
		if(TeamRadarNode* node = dynamic_cast<TeamRadarNode*>(item))
			boundingRect |= node->sceneBoundingRect();
	return boundingRect;
}

// dirlabel needs to know the depth of the whole tree to set opacity
int TeamRadarView::getDepth() const {
	return isLoaded() ? getRoot()->getDepth() : 0;
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
		if(TeamRadarNode* node = dynamic_cast<TeamRadarNode*>(item))
			node->setEffectsEnabled(enable);
	Setting::getInstance()->setValue("UseEffects", enable);
}