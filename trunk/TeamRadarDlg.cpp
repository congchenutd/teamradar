#include "TeamRadarDlg.h"
#include "Connection.h"
#include "Setting.h"
#include "PeerManager.h"
#include "PeerModel.h"
#include "TeamRadarEvent.h"
#include "ImageColorBoolDelegate.h"
#include "ImageColorBoolProxy.h"
#include "PlayerWidget.h"
#include <QHostAddress>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QProcess>
#include <QSqlQuery>
#include <QSortFilterProxyModel>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>

TeamRadarDlg::TeamRadarDlg(QWidget *parent) : QWidget(parent)
{
	peerManager = PeerManager::getInstance();
	ui.setupUi(this);

	// load settings
	setting = MySetting<Setting>::getInstance();
	ui.leServerAddress->setText (setting->getServerAddress());
	ui.sbPort         ->setValue(setting->getServerPort());
	ui.leUserName     ->setText (setting->getUserName());

	QPixmap pixmap = QPixmap(setting->getPhotoFilePath(getUserName())).scaled(128, 128);   // photo
	if(!pixmap.isNull())
		ui.labelImage->setPixmap(pixmap);
	ui.labelColor->setColor(setting->getColor("DefaultDeveloperColor"));    // color
	ui.gridLayout->update();

	// model
	model = peerManager->getPeerModel();

	// no myself, "true" before "false"
	model->setFilter(tr("Name <> \"%1\" order by Online Desc").arg(setting->getUserName()));

	ImageColorBoolProxy* proxy = new ImageColorBoolProxy(this);
	proxy->setColumnType(model->NAME,    ImageColorBoolProxy::NameColumn);
	proxy->setColumnType(model->COLOR,   ImageColorBoolProxy::ColorColumn);
	proxy->setColumnType(model->IMAGE,   ImageColorBoolProxy::ImageColumn);
	proxy->setColumnType(model->RECEIVE, ImageColorBoolProxy::BoolColumn);
	proxy->setImageColumn(model->IMAGE);
	proxy->setGrayImageBy(model->ONLINE);
	proxy->setSourceModel(model);

	ui.tvPeers->setModel(proxy);
	ImageColorBoolDelegate* delegate = new ImageColorBoolDelegate(proxy, ui.tvPeers);
	delegate->setCheckedImage  (QPixmap(":/Images/Checked.png"));
	delegate->setUncheckedImage(QPixmap(":/Images/Unchecked.png"));
	ui.tvPeers->setItemDelegate(delegate);
	ui.tvPeers->hideColumn(model->IMAGE);
	ui.tvPeers->hideColumn(model->ONLINE);
	resizeTable();

	connection = Connection::getInstance();
	connect(model,        SIGNAL(selected()), this, SLOT(resizeTable()));
	connect(ui.btConnect, SIGNAL(clicked()),  this, SLOT(onConnect()));
	connect(connection,   SIGNAL(connectionStatusChanged(bool)),
			this, SLOT(onConnectedToServer(bool)));

	onConnectedToServer(connection->isReadyForUse());   // init light
}

void TeamRadarDlg::save()
{
	// save settings
	QColor color = ui.labelColor->getColor();
	setting->setServerAddress(ui.leServerAddress->text());
	setting->setServerPort(ui.sbPort->value());
	setting->setUserName(getUserName());
	connection->setUserName(getUserName());
	setting->setColor("DefaultDeveloperColor", color);

	QString photoPath = setting->getPhotoFilePath(getUserName());
	if(ui.labelImage->pixmap() != 0)
		ui.labelImage->pixmap()->save(photoPath);  // save photo file

	// save my photo and color info in the db
	QString userName = getUserName();
	DeveloperInfo userInfo = model->getUserInfo(userName);
	userInfo.image = photoPath;
	userInfo.color = color;
	PeerModel::updateUser(userInfo);

	// send settings to the server
	registerPhoto();
	Sender::getInstance()->sendColorRegistration(color);

	// update local view
	PlayerWidget::getInstance()->setDeveloperImage(userName, QImage(photoPath));
	PlayerWidget::getInstance()->setDeveloperColor(userName, color);
}

void TeamRadarDlg::registerPhoto()
{
	QFile file(setting->getPhotoFilePath(getUserName()));
	if(!file.open(QFile::ReadOnly))
		return;

	QByteArray data = file.readAll();
	Sender::getInstance()->sendPhotoRegistration("png", data);
}

void TeamRadarDlg::resizeTable()
{
	ui.tvPeers->resizeRowsToContents();
	ui.tvPeers->resizeColumnsToContents();
}

QString TeamRadarDlg::getUserName() const {
	return ui.leUserName->text();
}

void TeamRadarDlg::contextMenuEvent(QContextMenuEvent* event)
{
	QModelIndexList idxes = ui.tvPeers->selectionModel()->selectedRows();
	if(idxes.isEmpty())
		return;

	QAction* actionDelete = new QAction("Delete", this);
	connect(actionDelete, SIGNAL(triggered()), this, SLOT(onDelete()));
	QMenu menu(this);
	menu.addAction(actionDelete);
	menu.exec(event->globalPos());
}

void TeamRadarDlg::onDelete()
{
	QModelIndexList idxes = ui.tvPeers->selectionModel()->selectedRows();
	if(idxes.isEmpty())
		return;

	if(QMessageBox::warning(this, tr("Warning"), tr("Really delete this entry?"),
		QMessageBox::Yes | QMessageBox::No)	== QMessageBox::No)
		return;

	model->removeRow(idxes.front().row());
}

void TeamRadarDlg::onShowHint() {
	ui.labelMessage->setText("*Restart to activate!");
}

void TeamRadarDlg::onConnectedToServer(bool connected) {
	ui.labelLight->setPixmap(connected ? QPixmap(":/Images/Green.png")
									   : QPixmap(":/Images/Red.png"));
	ui.groupClient->setEnabled(connected);
}

void TeamRadarDlg::onConnect()
{
	setting->setServerAddress(ui.leServerAddress->text());
	setting->setServerPort(ui.sbPort->value());
	connection->reconnect();
}
