#include "PlayerWidget.h"
#include <QStyle>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QTimer>
#include <QMessageBox>
#include "TeamRadarEvent.h"
#include "Setting.h"
#include "PeerManager.h"
#include "Connection.h"
#include "MessageCollector.h"
#include "RequestEventsDlg.h"
#include "ChatWindow.h"
#include "Analyzer.h"

PlayerWidget::PlayerWidget(QWidget *parent) :
	QWidget(parent)
{
	// models
	currentRow = -1;
	speed = 1.0;
	online = false;
	playing = false;
	peerManager = PeerManager::getInstance();

	modelPlaylist = new QStandardItemModel(this);
	modelPlaylist->setColumnCount(4);
	modelPlaylist->setHeaderData(DateTime,  Qt::Horizontal, tr("Time"));
	modelPlaylist->setHeaderData(UserName,  Qt::Horizontal, tr("User"));
	modelPlaylist->setHeaderData(EventType, Qt::Horizontal, tr("Event"));
	modelPlaylist->setHeaderData(Parameter, Qt::Horizontal, tr("Parameter"));

	// views
	ui.setupUi(this);
	QGraphicsScene *scene = new QGraphicsScene(this);
	scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	scene->setSceneRect(-300, -300, 600, 600);
	ui.graphicsView->setScene(scene);
	peerManager->setView(ui.graphicsView);

	ui.tvPlaylist->setModel(modelPlaylist);
	playIcon  = style()->standardIcon(QStyle::SP_MediaPlay);
	pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
	ui.btPlayPause->setIcon(playIcon);
	ui.btLoad     ->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	ui.btPlaylist ->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
	ui.btPlaylist ->setChecked(true);

	ui.slider->setMinimum(0);
	ui.splitter->setSizes(QList<int>() << height() * 0.7 << height() * 0.3);
//	ui.graphicsView->open("SavedGraph.graph");
	onOnline();
	onConnectedToServer(Connection::getInstance()->isReadyForUse());  // init light

	// signals
	connect(ui.btPlaylist,  SIGNAL(clicked(bool)), this, SLOT(onShowPlaylist(bool)));
	connect(ui.btEffects,   SIGNAL(clicked(bool)), this, SLOT(onEffects(bool)));
	connect(ui.btPlayPause, SIGNAL(clicked()),     this, SLOT(onPlayPause()));
	connect(ui.btLoad,      SIGNAL(clicked()),     this, SLOT(onLoad()));
	connect(ui.btOnline,    SIGNAL(clicked()),     this, SLOT(onOnline()));
	connect(ui.btDownload,  SIGNAL(clicked()),     this, SLOT(onDownload()));
	connect(ui.btAnalyze,   SIGNAL(clicked()),     this, SLOT(onAnalyze()));
	connect(ui.sbSpeed,     SIGNAL(valueChanged(double)),       this, SLOT(onSpeed(double)));
	connect(ui.tvPlaylist,  SIGNAL(clicked      (QModelIndex)), this, SLOT(onPlaylistClicked(QModelIndex)));
	connect(ui.tvPlaylist,  SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onPlaylistCoubleClicked(QModelIndex)));

	connect(MessageCollector::getInstance(), SIGNAL(localEvent(TeamRadarEvent)), this, SLOT(onEvent(TeamRadarEvent)));
	connect(Connection:: getInstance(), SIGNAL(connectionStatusChanged(bool)),  this, SLOT(onConnectedToServer(bool)));
	connect(peerManager, SIGNAL(userOnline(TeamRadarEvent)), this, SLOT(onEvent(TeamRadarEvent)));
	connect(Receiver::getInstance(), SIGNAL(newEvent      (TeamRadarEvent)), this, SLOT(onEvent(TeamRadarEvent)));
	connect(Receiver::getInstance(), SIGNAL(recentEvent   (TeamRadarEvent)), this, SLOT(onEvent(TeamRadarEvent)));
	connect(Receiver::getInstance(), SIGNAL(eventsResponse(TeamRadarEvent)), this, SLOT(onEventDownloaded(TeamRadarEvent)));
	connect(Receiver::getInstance(), SIGNAL(chatMessage(QString, QString)),  this, SLOT(onChatMessage(QString, QString)));
	connect(Receiver::getInstance(), SIGNAL(locationResponse(QString,QString)),
			this, SLOT(onLocationResponse(QString, QString)));
}

void PlayerWidget::onShowPlaylist(bool show)
{
	if(show)
		ui.splitter->setSizes(QList<int>() << 700 << 300);
	else
		ui.splitter->setSizes(QList<int>() << 100 << 0);
	ui.btPlaylist->setChecked(show);
}

void PlayerWidget::onPlayPause()
{
	if(playing)   // stop
	{
		ui.btPlayPause->setIcon(playIcon);
		playing = false;
	}
	else          // start
	{
		ui.btPlayPause->setIcon(pauseIcon);
		playing = true;
		play();
	}
}

void PlayerWidget::onLoad()
{
	QString fileName = QFileDialog::getOpenFileName(
				this, tr("Open Play List"), ".", tr("Playlist (*.list *.txt);;All files(*.*)"));
	if(fileName.isEmpty())
		return;

	QFile file(fileName);
	if(!file.open(QFile::ReadOnly))
		return;

	ui.graphicsView->loadDir(Setting::getInstance()->getRootPath());
	modelPlaylist->removeRows(0, modelPlaylist->rowCount());

	QTextStream is(&file);
	while(!is.atEnd())
	{
		QStringList sections = is.readLine().split("#");
		if(sections.size() == 4)
		{
			int lastRow = modelPlaylist->rowCount();
			modelPlaylist->insertRow(lastRow);
			for(int i=0; i<4; ++i)
				modelPlaylist->setData(modelPlaylist->index(lastRow, i), sections[i]);
		}
	}
	ui.tvPlaylist->resizeColumnsToContents();
	ui.slider->setMaximum(modelPlaylist->rowCount() - 1);
	modelPlaylist->sort(DateTime);
}

void PlayerWidget::onSpeed(double speed) {
	this->speed = speed;
}

void PlayerWidget::play()
{
	if(!playing)
		return;

	play(++currentRow);

	// ready for the next
	int nextRow = currentRow + 1;
	if(nextRow < modelPlaylist->rowCount())
	{
		QDateTime thisTime = modelPlaylist->data(modelPlaylist->index(currentRow, DateTime)).toDateTime();
		QDateTime nextTime = modelPlaylist->data(modelPlaylist->index(nextRow,    DateTime)).toDateTime();
		int duration = thisTime.secsTo(nextTime);
		QTimer::singleShot(duration * 1000 / speed, this, SLOT(play()));
	}
	else
		stop();
}

void PlayerWidget::play(int row)
{
	currentRow = row;
	selectRow(row);
	play(TeamRadarEvent(modelPlaylist->data(modelPlaylist->index(row, UserName)) .toString(),
						modelPlaylist->data(modelPlaylist->index(row, EventType)).toString(),
						modelPlaylist->data(modelPlaylist->index(row, Parameter)).toString()));
}

void PlayerWidget::play(const TeamRadarEvent& event)
{
	if(online && PeerModel::isBlocked(event.userName))
		return;

	if(!ui.graphicsView->humanExists(event.userName))   // this overrides CONNECTED event
		ui.graphicsView->addDeveloper(event.userName, peerManager->getImage(event.userName));

	if(event.eventType == "SAVE")
		ui.graphicsView->moveDeveloperTo(event.userName, event.parameters);
	else if(event.eventType == "MODE")
		ui.graphicsView->setDeveloperMode(event.userName, event.parameters);
	else if(event.eventType == "DISCONNECTED") {
		ui.graphicsView->removeDeveloper(event.userName);
	}
}

void PlayerWidget::reloadProject() {
	ui.graphicsView->loadDir(Setting::getInstance()->getRootPath());
}

void PlayerWidget::onPlaylistClicked(const QModelIndex& idx) {
	currentRow = idx.isValid() ? idx.row() : -1;
}

void PlayerWidget::onPlaylistCoubleClicked(const QModelIndex& idx) {
	if(idx.isValid())
		play(currentRow);
}

void PlayerWidget::onOnline()
{
	online = !online;
	ui.btOnline->setIcon(online ? QIcon(":/Images/Connect.png") : QIcon(":/Images/Disconnect.png"));
	ui.btPlaylist ->setHidden(online);
	ui.btPlayPause->setHidden(online);
	ui.slider     ->setHidden(online);
	ui.sbSpeed    ->setHidden(online);
	ui.btLoad     ->setHidden(online);
	ui.btDownload ->setHidden(online);
	ui.btAnalyze  ->setHidden(online);
	onShowPlaylist(!online);
	ui.graphicsView->setEffectsEnabled(!online);
	reloadProject();
}

void PlayerWidget::onEffects(bool show) {
	ui.graphicsView->setEffectsEnabled(show);
}

void PlayerWidget::stop()
{
	ui.btPlayPause->setIcon(playIcon);
	currentRow = -1;
	playing = false;
}

void PlayerWidget::selectRow(int row)
{
	ui.tvPlaylist->selectRow(row);
	ui.slider->setValue(row);
}

void PlayerWidget::closeEvent(QCloseEvent*)
{
//	ui.graphicsView->save("SavedGraph.graph");
	ChatWindow::saveAllHistory();
}

void PlayerWidget::resizeEvent(QResizeEvent*) {
	ui.graphicsView->autoScale();
}

void PlayerWidget::onEvent(const TeamRadarEvent& event) {
	if(online)
		play(event);
}

void PlayerWidget::onDownload()
{
	RequestEventsDlg dlg(this);
	if(dlg.exec() == QDialog::Accepted)
	{
		modelPlaylist->removeRows(0, modelPlaylist->rowCount());   // remove current, request new
		Sender::getInstance()->sendEventRequest(dlg.getUserList(),
												dlg.getEventList(),
												dlg.getStartTime(),
												dlg.getEndTime(),
												dlg.getPhases(),
												dlg.getFuzziness());
	}
}

void PlayerWidget::onConnectedToServer(bool connected) {
	ui.labelConnection->setPixmap(connected ? QPixmap(":/Images/Green.png")
											: QPixmap(":/Images/Red.png"));
}

void PlayerWidget::onEventDownloaded(const TeamRadarEvent& event)
{
	if(online)    // for offline only
		return;

	int lastRow = modelPlaylist->rowCount();
	modelPlaylist->insertRow(lastRow);
	modelPlaylist->setData(modelPlaylist->index(lastRow, 0), event.time.toString(Setting::dateTimeFormat));
	modelPlaylist->setData(modelPlaylist->index(lastRow, 1), event.userName);
	modelPlaylist->setData(modelPlaylist->index(lastRow, 2), event.eventType);
	modelPlaylist->setData(modelPlaylist->index(lastRow, 3), event.parameters);

	ui.tvPlaylist->resizeColumnsToContents();
	ui.slider->setMaximum(modelPlaylist->rowCount() - 1);
	modelPlaylist->sort(DateTime);
}

// received a chat message from peerName
void PlayerWidget::onChatMessage(const QString& peerName, const QString& content) {
	if(HumanNode* human = ui.graphicsView->findDeveloper(peerName))
		human->chat(content);
}

void PlayerWidget::onAnalyze()
{
	Analyzer analyzer(modelPlaylist, this);
	analyzer.exec();
}

PlayerWidget::~PlayerWidget()
{
	ChatWindow::saveAllHistory();
	ChatWindow::closeAllWindows();
	instance = 0;
}

void PlayerWidget::showEvent(QShowEvent*)
{
	static bool firstTime = true;
	if(firstTime)
	{
		peerManager->refreshUserList();
		firstTime = false;
	}
}

PlayerWidget* PlayerWidget::getInstance()
{
	if(instance == 0)
		instance = new PlayerWidget;
	return instance;
}

PlayerWidget* PlayerWidget::instance = 0;
