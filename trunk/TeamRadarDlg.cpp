#include "TeamRadarDlg.h"
#include "Connection.h"
#include "Setting.h"
#include "PeerManager.h"
#include "PeerModel.h"
#include "TeamRadarEvent.h"
#include "ImageColorBoolDelegate.h"
#include "ImageColorBoolProxy.h"
#include <QHostAddress>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QProcess>
#include <QFileDialog>
#include <QColorDialog>
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
	ui.leUserName->setText(setting->getUserName().isEmpty() ? 
											guessUserName() : setting->getUserName());

	QPixmap pixmap = QPixmap(setting->getPhotoFilePath(getUserName())).scaled(128, 128);   // photo
	if(!pixmap.isNull())
		ui.labelImage->setPixmap(pixmap);
	setColor(setting->getColor("DefaultDeveloperColor"));            // color

	ui.labelAbout->setText(tr("<H1 align=\"center\">Team Radar</H1>"
							  "<P align=\"center\">A workspace awareness plugin</P>"
							  "<P align=\"center\">Cong Chen &lt;CongChenUTD@Gmail.com&gt;</P>"
							  "<P align=\"center\">Build on %1</P>").arg(setting->getCompileDate()));
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

	connect(model,      SIGNAL(selected()), this, SLOT(resizeTable()));
    connect(ui.btImage, SIGNAL(clicked()),  this, SLOT(onSetImage()));
	connect(ui.btColor, SIGNAL(clicked()),  this, SLOT(onSetColor()));
	connect(ui.leServerAddress, SIGNAL(textEdited(QString)), this, SLOT(onShowHint()));
	connect(ui.sbPort,          SIGNAL(valueChanged(int)),   this, SLOT(onShowHint()));
	connect(Connection::getInstance(), SIGNAL(connectionStatusChanged(bool)), this, SLOT(onConnectedToServer(bool)));
	
	onConnectedToServer(Connection::getInstance()->isReadyForUse());   // init light
}

void TeamRadarDlg::save()
{
	// save settings
	setting->setServerAddress(ui.leServerAddress->text());
	setting->setServerPort(ui.sbPort->value());
	setting->setUserName(getUserName());
	setting->setColor("DefaultDeveloperColor", color);

	QString photoPath = setting->getPhotoFilePath(getUserName());
	if(ui.labelImage->pixmap() != 0)
		ui.labelImage->pixmap()->save(photoPath);  // save photo file

	// save my photo and color info in the db
	DeveloperInfo userInfo = model->getUserInfo(getUserName());
	userInfo.image = photoPath;
	userInfo.color = color;
	model->updateUser(userInfo);

	// send settings to the server
	registerPhoto();
	Sender::getInstance()->sendColorRegistration(color);
}

// search the environmental variables for user name
QString TeamRadarDlg::guessUserName() const
{
	QStringList envVariables;
	envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
				 << "HOSTNAME.*" << "DOMAINNAME.*";
	QString result;
	QStringList environment = QProcess::systemEnvironment();
	foreach(QString string, envVariables)
	{
		int index = environment.indexOf(QRegExp(string));
		if(index != -1)
		{
			QStringList stringList = environment.at(index).split('=');
			if(stringList.size() == 2)
			{
				result = stringList.at(1).toUtf8();
				break;
			}
		}
	}

	return result.isEmpty() ? "Unknown" : result;
}

void TeamRadarDlg::onSetImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose Image"), ".",
                                                    "Images (*.png *.jpg *.bmp *.ico)");
    if(!fileName.isEmpty())
		ui.labelImage->setPixmap(QPixmap(fileName).scaled(128, 128));
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

void TeamRadarDlg::onSetColor() {
	setColor(QColorDialog::getColor(color));
}

QString TeamRadarDlg::getUserName() const {
	return ui.leUserName->text();
}

void TeamRadarDlg::setColor(const QColor& clr)
{
	if(clr.isValid())
	{
		color = clr;
		QPixmap pixmap(ui.labelColor->size());
		pixmap.fill(color);
		ui.labelColor->setPixmap(pixmap);
	}
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
	ui.labelMessage->setText("Restart to activate!");
}

void TeamRadarDlg::onConnectedToServer(bool connected) {
	ui.labelLight->setPixmap(connected ? QPixmap(":/Images/Green.png") 
									   : QPixmap(":/Images/Red.png"));
}
