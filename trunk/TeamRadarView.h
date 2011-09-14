#ifndef TEAMRADARVIEW_H
#define TEAMRADARVIEW_H

#include <QtGui/QGraphicsView>
#include <QMap>
#include "Node.h"

class TeamRadarNode;
class QAction;
class QXmlStreamWriter;
class QXmlStreamReader;
class Engine;
class HumanNode;
struct TeamRadarEvent;

typedef QMap<QString, HumanNode*> Humans;

// A force-directed layout implementation
class TeamRadarView : public QGraphicsView
{
	Q_OBJECT

public:
	TeamRadarView(QWidget *parent = 0);
	void itemMoved();                                         // start layouting
	void loadDir(const QString& dirPath, int maxLevel = 0);   // load the whole view
	void open(const QString& fileName);                       // open graph file
	void save(const QString& fileName);                       // save to graph
	void setShowLabels(bool show);                            // all nodes show label
	qreal getScale() const { return scaleFactor; }            // labels use it to maintain font size
	void expandNode(TeamRadarNode* node, int maxLevel);       // expand the node from FS
	void clear();
	TeamRadarNode* getRoot() const    { return rootNode; }
	bool isLoaded() const { return getRoot() != 0; }
	void setRoot(TeamRadarNode* root) { rootNode = root; }
	void setAfterImageDuration(int seconds);
	void autoScale();                                         // auto scale
	int  getDepth() const;                                    // depth of the tree
	void centerTree(TeamRadarNode* node = 0);                 // center to node
	const Humans& getHumans() const { return humans; }
	void setEffectsEnabled(bool enable);
	TeamRadarNode* createNode(bool isDir, const QString& name, TeamRadarNode* owner);     // factory
	bool humanExists(const QString& name);

	void addDeveloper(const QString& name, const QString& image);
	void moveDeveloperTo(const QString& name, const QString& relativePath);
	void removeDeveloper(const QString& name);
	void setDeveloperMode(const QString& developerName, const QString& mode);
	HumanNode* findDeveloper(const QString& name);

protected:
	void keyPressEvent(QKeyEvent*   event);
	void wheelEvent   (QWheelEvent* event);
	void contextMenuEvent (QContextMenuEvent* event);
	void mouseMoveEvent   (QMouseEvent* event);
	void mousePressEvent  (QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void closeEvent(QCloseEvent*);
	bool viewportEvent(QEvent *event);

private:
	void resetScale();
	void scaleBy(qreal scaleFactor);
	void scaleTo(qreal scaleFactor);
	void  createNodesFromFS(const QString& path, TeamRadarNode* owner, int maxLevel);  // recursive
	void  saveNodeToXML(QXmlStreamWriter& xml, const TeamRadarNode* node);
	TeamRadarNode* loadNodeFromXML(QXmlStreamReader& xml, TeamRadarNode* parent, bool isDir);
	void moveCanvasBy(int x, int y);
	void moveCanvasBy(const QPointF& vec);
	void initActions();                          // init menu actions for nodes
	QRectF nodesBoundingRect() const;

private slots:
	void onAddDir();                             // work with menu
	void onAddFile();
	void onDel();
	void onExpand();
	void onCollapse();
	void onPin(bool pin);
	void onWorkOn();
	void onChat();

public:
	QAction* actionAddDir;
	QAction* actionAddFile;
	QAction* actionDel;
	QAction* actionExpand;
	QAction* actionCollapse;
	QAction* actionPin;
	QAction* actionWorkOn;
	QAction* actionChat;

protected:
	qreal scaleFactor;
	TeamRadarNode* currentNode;        // the node menu actions work on
	QPoint lastMousePos;      // for dragging
	bool dragging;
	Engine* engine;           // layout algorithm
	bool picking;             // after "Work On" action is triggered
	HumanNode* currentHuman;
	Humans humans;
	TeamRadarNode* rootNode;
};


#endif
