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

PlayerWidget::PlayerWidget(QWidget *parent) :
	QWidget(parent)
{
	currentRow = -1;
	speed = 1.0;
	online = false;
	playing = false;
	peerManager = PeerManager::getInstance();

	peerManager->setDeveloperColor("Carl", Qt::blue);
	peerManager->setDeveloperColor("Mike", Qt::cyan);
	peerManager->setDeveloperColor("Jane", Qt::magenta);
	peerManager->setImage("Carl", ":/Images/Head_Boss.png");
	peerManager->setImage("Mike", ":/Images/Head_Male.png");
	peerManager->setImage("Jane", ":/Images/Head_Female.png");

	model = new QStandardItemModel(this);
	model->setColumnCount(4);
	model->setHeaderData(DateTime,  Qt::Horizontal, tr("Time"));
	model->setHeaderData(UserName,  Qt::Horizontal, tr("User"));
	model->setHeaderData(EventType, Qt::Horizontal, tr("Event"));
	model->setHeaderData(Parameter, Qt::Horizontal, tr("Parameter"));

	ui.setupUi(this);
	QGraphicsScene *scene = new QGraphicsScene(this);
	scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	scene->setSceneRect(-300, -300, 600, 600);
	ui.graphicsView->setScene(scene);

	ui.tvPlaylist->setModel(model);
	playIcon  = style()->standardIcon(QStyle::SP_MediaPlay);
	pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
	ui.btPlayPause->setEnabled(false);
	ui.btPlayPause->setIcon(playIcon);
	ui.btLoad     ->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	ui.btPlaylist ->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
	ui.btPlaylist ->setChecked(true);

	ui.slider->setEnabled(false);
	ui.splitter->setSizes(QList<int>() << height() * 0.7 << height() * 0.3);
//	ui.graphicsView->open("SavedGraph.graph");
	onOnline();

	connect(ui.btPlaylist,  SIGNAL(clicked(bool)), this, SLOT(onShowPlaylist(bool)));
	connect(ui.btPlayPause, SIGNAL(clicked()),     this, SLOT(onPlayPause()));
	connect(ui.btLoad,      SIGNAL(clicked()),     this, SLOT(onLoad()));
	connect(ui.btOnline,    SIGNAL(clicked()),     this, SLOT(onOnline()));
	connect(ui.btEffects,   SIGNAL(clicked(bool)), this, SLOT(onEffects(bool)));
	connect(ui.sbSpeed,     SIGNAL(valueChanged(double)), this, SLOT(onSpeed(double)));
	connect(ui.tvPlaylist, SIGNAL(clicked(QModelIndex)),       this, SLOT(onPlaylistClicked(QModelIndex)));
	connect(ui.tvPlaylist, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onPlaylistCoubleClicked(QModelIndex)));

	connect(Connection::getInstance(),       SIGNAL(newMessage(QString)), this, SLOT(onNewMessage(QString)));
	connect(MessageCollector::getInstance(), SIGNAL(localEvent(QString)), this, SLOT(onNewMessage(QString)));
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

	ui.graphicsView->loadDir(Setting::getInstance()->value("RootPath").toString(), 0);
	model->removeRows(0, model->rowCount());
	
	QTextStream is(&file);
	while(!is.atEnd())
	{
		QStringList sections = is.readLine().split("#");
		if(sections.size() == 4)
		{
			int lastRow = model->rowCount();
			model->insertRow(lastRow);
			for(int i=0; i<4; ++i)
				model->setData(model->index(lastRow, i), sections[i]);
		}
	}
	ui.btPlayPause->setEnabled(true);
	ui.tvPlaylist->resizeColumnsToContents();
	ui.slider->setMinimum(0);
	ui.slider->setMaximum(model->rowCount() - 1);
	model->sort(DateTime);
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
	if(nextRow < model->rowCount())
	{
		QDateTime thisTime = model->data(model->index(currentRow, DateTime)).toDateTime();
		QDateTime nextTime = model->data(model->index(nextRow,    DateTime)).toDateTime();
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

	// play the event
	play(TeamRadarEvent(model->data(model->index(row, UserName)) .toString(),
						model->data(model->index(row, EventType)).toString(),
						model->data(model->index(row, Parameter)).toString()));
}

void PlayerWidget::play(const TeamRadarEvent& event)
{
	if(event.eventType == "SAVE")
		ui.graphicsView->moveDeveloperTo(event.userName, event.parameter);
	else if(event.eventType == "MODE")
		ui.graphicsView->setDeveloperMode(event.userName, event.parameter);
	else if(event.eventType == "CONNECTED")
		ui.graphicsView->addDeveloper(event.userName, peerManager->getImage(event.userName));
	else if(event.eventType == "DISCONNECTED")
		ui.graphicsView->removeDeveloper(event.userName);
	else if(event.eventType == "OPENPROJECT")
	{
		if(event.userName != Setting::getInstance()->getUserName())
			return;
		Setting::getInstance()->setValue("RootPath", event.parameter);
		ui.graphicsView->loadDir(event.parameter);
		play(TeamRadarEvent(event.userName, "CONNECTED", ""));  // add myself
	}
}

void PlayerWidget::onPlaylistClicked(const QModelIndex& idx) {
	currentRow = idx.isValid() ? idx.row() : -1;
}

void PlayerWidget::onPlaylistCoubleClicked(const QModelIndex& idx)
{
	if(idx.isValid())
		play(currentRow);
}

void PlayerWidget::onOnline()
{
	if(online)   // go offline
		ui.btOnline->setIcon(QIcon(":/Images/Disconnect.png"));
	else         // go online
		ui.btOnline->setIcon(QIcon(":/Images/Connect.png"));
	online = !online;
	ui.btPlayPause->setEnabled(!online);
	ui.btLoad     ->setEnabled(!online);
	ui.sbSpeed    ->setEnabled(!online);
	onShowPlaylist(!online);
	ui.graphicsView->setEffectsEnabled(!online);

	if(online)
	{
		Setting* setting = Setting::getInstance();
		if(!ui.graphicsView->isLoaded())
			ui.graphicsView->loadDir(setting->value("RootPath").toString(), 0);
//		play(TeamRadarEvent(setting->getUserName(), "CONNECTED", ""));  // add myself
	}
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

void PlayerWidget::closeEvent(QCloseEvent*) {
	ui.graphicsView->save("SavedGraph.graph");
}

void PlayerWidget::resizeEvent(QResizeEvent*) {
	ui.graphicsView->autoScale();
}

void PlayerWidget::onNewMessage(const QString& message)
{
	if(!online)
		return;

	// username, event, parameters
	QStringList sections = message.split("#");
	if(sections.size() != 3)
		return;
	TeamRadarEvent event(sections[0], sections[1], sections[2]);
	play(event);
}