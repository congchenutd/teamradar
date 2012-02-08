#ifndef NODE_H
#define NODE_H

#include <QGraphicsObject>
#include <QImage>
#include <QFileInfo>
#include "Edge.h"

class TeamRadarView;
class ForceDirectedLayoutView;
class Label;
class LayeredEngine;
class HumanNode;

typedef QList<TeamRadarNode*> Nodes;

// A node represent an item on the canvas
// A node is connected to its parent with an edge, parent being the source of the edge
class TeamRadarNode : public QGraphicsObject
{
	Q_OBJECT
public:
	TeamRadarNode(TeamRadarNode* owner, const QString& name = QString(), int level = 0);
	virtual ~TeamRadarNode() {}      // do not use dtr to destroy, scene()->clear() kills them all
	enum { Type = UserType + 11 };
	int type() const { return Type; }

	void setShowLabel(bool show);
	void setPinned(bool pin) { pinned = pin;  }    // not movable by force if pinned
	bool isPinned() const    { return pinned; }
	TeamRadarNode* getOwner() const { return owner; }
	QString getName() const { return name; }
	QColor getColor() const          { return color; }
	void   setColor(const QColor& c) { color = c;    }
	int  getLevel() const { return level; }
	void setLevel(int l)  { level = l; }
	void addEdge   (Edge* edge);
	void removeEdge(Edge* edge);
	const Edges& getEdges() const { return edges; }
	void updateSize();                              // recalculate # of nodes in this subtree
	int getSize()  const { return size;  }
	int getDepth() const { return depth; }
	TeamRadarNode* findChild(const QString& name) const;     // find direct child by name
	void showLabel();
	int getWidth() const { return 2 * getRadius(); }
	void loosenEdgeToOwner();                       // "pick the apple"
	void fastenEdgeToOwner();                       // "release the apple"
	void removeEdgeToOwner();
	Edge* findOwnerEdge() const;                    // find the edge connecting to the owner
	void randomize();                               // randomize position
	Nodes getChildren() const;
	QFileInfo findMatchingPath(const QString& filePath);  // max matching path in FS starting from this node
	void setEffectsEnabled(bool enable);
	void detectConflict();                          // when an humannode enter/leave a node
	QList<HumanNode*> getHumans() const;
	bool advance();                                 // update position, return true if moved

	virtual QMenu& getContextMenu() const = 0;
	virtual int    getRadius() const { return 8; }
	virtual void   setNewPos(const QPointF& p) { newPos = p; }  // get ready to update position
	virtual void   removeChildren();
	virtual void   randomizeChildren() {}                // randomize children's position
	virtual bool   expandable()  const { return false; }
	virtual bool   collapsable() const { return false; }
	virtual void   updateDepth() { depth = 0; }          // used for dirnode only
	virtual double getForce()   const;
	virtual Nodes  getPushers() const;                 // for localized engine
	virtual void   hideLabel();

	// use a separate function to delete, instead of using destructor
	// because scene()->clear() destroys items in no order, may cause double kill
	virtual void suicide();

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	static void  setGraph(TeamRadarView* v) { view = v; }

protected:
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant&);    // i.e. dragged
	virtual int getNetSize() const { return 1; }   // excluding children
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent*);

	Nodes getAncestors() const;
	TeamRadarNode* getRoot() const;
	void drawSelectionRect(QPainter* painter);

public:
	enum {MaxLevel = 100};

protected:
	static TeamRadarView* view;
	Edges   edges;
	QPointF newPos;     // buffer before change, so that we can detect convergence
	QString name;       // path for file/dir node, name for human
	int     level;      // 0 being root
	int     depth;      // depth of the subtree
	QColor  color;      // human uses it for light trail, also used for label
	bool    pinned;     // unmovable by force if pinned
	int     size;       // size of the subtree
	TeamRadarNode* owner;
	Label*         label;
};

class DirNode : public TeamRadarNode
{
	Q_OBJECT
public:
	DirNode(TeamRadarNode* owner, const QString& name);
	enum { Type = UserType + 12 };
	int type() const { return Type; }

	virtual QMenu& getContextMenu() const;
	virtual void randomizeChildren();
	virtual bool expandable()  const { return directChildrenCount() == 0; }
	virtual bool collapsable() const { return directChildrenCount() > 0;  }
	virtual void updateDepth();

protected:
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*);  // expand/collapse

private:
	int directChildrenCount() const;   // # of sons, used to determine if expanded
};

class FileNode : public TeamRadarNode
{
public:
	FileNode(TeamRadarNode* owner, const QString& name);
	enum { Type = UserType + 13 };
	int type() const { return Type; }

	virtual QMenu& getContextMenu() const;
};

class LightTrail;
class ModeTag;

class HumanNode : public TeamRadarNode
{
	Q_OBJECT

public:
	HumanNode(const QString& name, const QImage& img, TeamRadarNode* owner = 0);
	enum { Type = UserType + 14 };
	int type() const { return Type; }

	void setWorkOn(const QString& filePath);
	void lightTrailDied(LightTrail* trail);     // reset lightTrail pointer
	void setMode(const QString& mode);
	void enterCanvas();
	void leaveCanvas();
	void showConflict(bool show);
	void updateOwner(bool expandable = true);    // move to a new owner
	void chat(const QString& content = QString());
	void setImage(const QImage& img) { image = img; }

	virtual void   setNewPos(const QPointF& p);
	virtual QMenu& getContextMenu() const;
	virtual int    getRadius() const { return 20; }
	virtual void   removeChildren() {}
	virtual Nodes  getPushers() const;
	virtual void   hideLabel();

	// called by owner's removeChildren(), meaning owner collapsed, and needs to find new owner
	virtual void suicide() { updateOwner(false); }

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);

protected:
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);

private:
	TeamRadarNode* findOwner(TeamRadarNode* parent, const QString& path, bool expandable = true);   // find the node it works on
	void leaveAfterimage();
	void detachFromOwner();

private slots:
	void onDead();

protected:
	QImage image;
	QString workOn;                    // file path it works on
	LightTrail* lightTrail;
	ModeTag*    modeTag;
	bool dying;

	static const double oneSlice;
};

class AfterimageNode : public HumanNode
{
	Q_OBJECT

public:
	static void setFadeOutDuration(int duration) { fadeOutDuration = duration; }

	AfterimageNode(const QString& name, const QImage& img, TeamRadarNode* owner = 0);
	enum { Type = UserType + 15 };
	int type() const { return Type; }
	virtual void hideLabel() { return TeamRadarNode::hideLabel(); }

	//virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);

private slots:
	void onDead();

private:
	static int fadeOutDuration;
};


#endif
