#ifndef MODETAG_H
#define MODETAG_H

#include <QGraphicsObject>

class TeamRadarNode;

class ModeTag : public QGraphicsObject
{
public:
	ModeTag(TeamRadarNode* n);
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);
	virtual QImage  getImage();
	virtual QString getName()  const = 0;

protected:
	enum {width = 24};
	TeamRadarNode* node;
};

class NullTag : public ModeTag
{
public:
	NullTag(TeamRadarNode* n) : ModeTag(n) {}
	virtual QImage  getImage() const { return QImage(); }
	virtual QString getName()  const { return "Null"; }
};

class WelcomeModeTag : public ModeTag
{
public:
	WelcomeModeTag(TeamRadarNode* n) : ModeTag(n) {}
	virtual QString getName() const { return "Welcome"; }
};

class EditModeTag : public ModeTag
{
public:
	EditModeTag(TeamRadarNode* n) : ModeTag(n) {}
	virtual QString getName() const { return "Edit"; }
};

class DesignModeTag : public ModeTag
{
public:
	DesignModeTag(TeamRadarNode* n) : ModeTag(n) {}
	virtual QString getName() const { return "Design"; }
};

class DebugModeTag : public ModeTag
{
public:
	DebugModeTag(TeamRadarNode* n) : ModeTag(n) {}
	virtual QString getName() const { return "Debug"; }
};

class ProjectsModeTag : public ModeTag
{
public:
	ProjectsModeTag(TeamRadarNode* n) : ModeTag(n) {}
	virtual QString getName() const { return "Projects"; }
};

class HelpModeTag : public ModeTag
{
public:
	HelpModeTag(TeamRadarNode* n) : ModeTag(n) {}
	virtual QString getName() const { return "Help"; }
};

// a special mode, not in Qt Creator
// for conflict alert
class AlertModeTag : public ModeTag
{
public:
	AlertModeTag(TeamRadarNode* n) : ModeTag(n) {}
	virtual QString getName() const { return "Conflict"; }
};


#endif // MODETAG_H
