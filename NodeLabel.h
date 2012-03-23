#ifndef LABEL_H
#define LABEL_H

#include <QGraphicsObject>
#include <QFont>

namespace TeamRadar {

class TeamRadarNode;
class TeamRadarView;

class NodeLabel : public QGraphicsObject
{
public:
	static void setGraph(TeamRadarView* g);
	NodeLabel(TeamRadarNode* n);
	virtual ~NodeLabel() {}
	enum { Type = UserType + 21 };
	int type() const { return Type; }

	void addToScene(QGraphicsScene* scene);
	void setEffectsEnabled(bool enable);

	virtual void show();    // they are not virtual in QGraphicsObject
	virtual void update();

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);

protected:
	QRect textRect() const;

protected:
	static TeamRadarView* graph;
	enum {margin = 1, fontSize = 12};
	TeamRadarNode* node;        // the node it belongs to
	QFont   font;
	QString text;
};

class FileLabel : public NodeLabel
{
public:
	FileLabel(TeamRadarNode* n);
	enum { Type = UserType + 22 };
	int type() const { return Type; }
};

class DirLabel : public FileLabel
{
public:
	DirLabel(TeamRadarNode* n);
	enum { Type = UserType + 23 };
	int type() const { return Type; }
};

// nothing, but a new type
class HumanLabel : public FileLabel
{
public:
	HumanLabel(TeamRadarNode* n);
	enum { Type = UserType + 24 };
	int type() const { return Type; }
};


} // namespace TeamRadar

#endif // LABEL_H
