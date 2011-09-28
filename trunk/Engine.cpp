#include "Engine.h"
#include "Node.h"
#include "TeamRadarView.h"
#include "Setting.h"
#include <math.h>
#include <QQueue>
#include <QMessageBox>
#include <QTimerEvent>
#include <QGraphicsScene>

Engine* Engine::instance = 0;

Engine::Engine(TeamRadarView* v)
{
	view = v;
	timerID = 0;
	instance = this;
}

void Engine::start() {
	if(timerID == 0)
	{
		timerID = startTimer(10);
//		time.start();
	}
}

void Engine::stop()
{
//	QMessageBox::information(0, tr("Done"), tr("Seconds = %1").arg(time.elapsed()));
	killTimer(timerID);
	timerID = 0;
}

void Engine::timerEvent(QTimerEvent*)
{
	if(!view->isLoaded())   // the tree is not loaded
		return stop();

	itemsMoved = false;
	step();
	if(!itemsMoved)  // converged
		stop();
}

void Engine::pull(TeamRadarNode* node, qreal& xvel, qreal& yvel)
{
	foreach(Edge* edge, node->getEdges())
		if(edge->getDest() == node)                  // find owner's edge
		{
			QPointF vec = node->mapToItem(edge->getSource(), 0, 0);
			xvel -= edge->getForce() * vec.x();
			yvel -= edge->getForce() * vec.y();
		}
}

void Engine::calculateForces(TeamRadarNode* node)
{
	// ignore root, grabbed, and pinned node
	if(node->getOwner() == 0 || view->scene()->mouseGrabberItem() == node || node->isPinned()) {
		node->setNewPos(node->pos());
		return;
	}

	qreal xvel = 0;
	qreal yvel = 0;
	push(node, xvel, yvel);
	pull(node, xvel, yvel);

	// ignore slight move
	qreal sensitivity = Setting::getInstance()->getThreshold();
	if(qAbs(xvel) < sensitivity && qAbs(yvel) < sensitivity)
		xvel = yvel = 0;
	node->setNewPos(node->pos() + QPointF(xvel, yvel));
}

////////////////////////////////////////////////////////////////////////
LayeredEngine::LayeredEngine(TeamRadarView *v)
: Engine(v) {}

void LayeredEngine::step()
{
	QQueue<TeamRadarNode*> queue;                    // BFS
	queue.enqueue(view->getRoot());
	while(!queue.isEmpty())
	{
		TeamRadarNode* node = queue.dequeue();

		calculateForces(node);    // calculate
		if(node->advance())       // move
			itemsMoved = true;

		queue << node->getChildren();
	}
}

void LayeredEngine::push(TeamRadarNode* node, qreal& xvel, qreal& yvel)
{
	Nodes pushers = node->getPushers();
	foreach(TeamRadarNode* pusher, pushers)
	{
		QPointF vec = node->mapToItem(pusher, 0, 0);
		qreal dx = vec.x() == 0.0 ? qrand() / node->getRadius() : vec.x();
		qreal dy = vec.y() == 0.0 ? qrand() / node->getRadius() : vec.y();

		double d = sqrt(dx * dx + dy * dy);
		if(d > 0)
		{
			double pushForce = 15*sqrt(pusher->getForce() * node->getForce());
			if(pusher->getLevel() < node->getLevel())   // parent
				pushForce *= 2;
			else                                        // sibling
				pushForce *= 1;

			double dd = qMax((double)abs(d - node->getRadius()), (double)node->getRadius());
			xvel += pushForce * dx / dd / dd;
			yvel += pushForce * dy / dd / dd;
		}
	}
}
