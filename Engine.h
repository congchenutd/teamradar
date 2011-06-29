#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QTimer>
#include <QTime>

class TeamRadarView;
class TeamRadarNode;

class Engine : public QObject
{
	Q_OBJECT

public:
	Engine(TeamRadarView* v);
	virtual ~Engine() {}
	static Engine* getEngine() { return instance; }    // not a real singleton

	void start();
	void stop();
	bool isRunning() { return timerID != 0; }

protected:
	virtual void timerEvent(QTimerEvent* event);       // faster than QTimer
	virtual void step() = 0;                           // one iteration for one timer event
	virtual void calculateForces(TeamRadarNode* node);          // calculate the forces for one node
	virtual void push(TeamRadarNode* node, qreal& xvel, qreal& yvel) = 0;
	virtual void pull(TeamRadarNode* node, qreal& xvel, qreal& yvel);

protected:
	TeamRadarView* view;
	int   timerID;
	bool  itemsMoved;
	QTime time;
	static Engine* instance;
};

// localized engine, considering only siblings and ancestors
class LayeredEngine : public Engine
{
public:
	LayeredEngine(TeamRadarView* v);

protected:
	virtual void step();
	virtual void push(TeamRadarNode* node, qreal& xvel, qreal& yvel);
};

#endif // ENGINE_H
