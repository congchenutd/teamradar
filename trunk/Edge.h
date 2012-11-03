#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsObject>

namespace TeamRadar {

class TeamRadarNode;
class Edge;

typedef QList<Edge*> Edges;

// directed edge connecting a parent node (source) and a child node (dest)
class Edge : public QGraphicsObject
{
public:
    Edge(TeamRadarNode* sourceNode, TeamRadarNode* destNode, double force = 1.0);
	virtual ~Edge() {}
	enum { Type = UserType + 31 };
	int type() const { return Type; }

	TeamRadarNode*  getSource() const { return source; }
	TeamRadarNode*  getDest()   const { return dest;   }
	void setForce(double f) { force = f; }
	void adjust();                               // adjust the positions of the ends

	virtual double getForce() const;
	virtual QRectF boundingRect() const;

protected:
	TeamRadarNode* source;
	TeamRadarNode* dest;
	QPointF sourcePoint;
	QPointF destPoint;
	double  force;
	QColor  color;
};

class TreeEdge : public Edge
{
public:
	TreeEdge(TeamRadarNode *sourceNode, TeamRadarNode *destNode, double force = 1.0);
	enum { Type = UserType + 32 };
	int type() const { return Type; }

	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);
};

class HumanEdge : public Edge
{
public:
	HumanEdge(TeamRadarNode *sourceNode, TeamRadarNode *destNode, double force = 0.5);
	enum { Type = UserType + 33 };
	int type() const { return Type; }

	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);  // invisible
};

} // namespace TeamRadar

#endif
