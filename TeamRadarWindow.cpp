#include "TeamRadarWindow.h"
#include "Connection.h"
#include "Setting.h"
#include "PeerManager.h"
#include "PeerModel.h"
#include <QHostAddress>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QProcess>
#include <QFileDialog>
#include <QColorDialog>
#include <QSqlQuery>

TeamRadarWindow::TeamRadarWindow(QWidget *parent) : QDialog(parent)
{
	imageChanged = false;
	peerManager = PeerManager::getInstance();
    ui.setupUi(this);

	setting = MySetting<Setting>::getInstance();
	ui.leServerAddress->setText(setting->getServerAddress());
	ui.sbPort->setValue(setting->getServerPort());
	userName = setting->getUserName();
	if(userName.isEmpty())
		userName = guessUserName();
	ui.leUserName->setText(userName);

	QPixmap pixmap = QPixmap(userName + ".png").scaled(128, 128);
	ui.labelImage->setPixmap(pixmap);

	model = peerManager->getPeerModel();
	ui.tvPeers->setModel(model);
	ui.tvPeers->hideColumn(PEER_IMAGE);
	ui.tvPeers->hideColumn(PEER_ONLINE);
	resizeTable();

    connect(ui.btImage,  SIGNAL(clicked()),                  this, SLOT(onSetImage()));
	connect(ui.tvPeers,  SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onEditPeer(QModelIndex)));
	connect(peerManager, SIGNAL(userListChanged(QString)),   this, SLOT(onUserListChanged()));

	peerManager->refreshUserList();
}

void TeamRadarWindow::accept()
{
	setting->setServerAddress(ui.leServerAddress->text());
	setting->setServerPort(ui.sbPort->value());
	setting->setUserName(ui.leUserName->text());

	// register the photo on server
	if(imageChanged)
		registerPhoto();
	QDialog::accept();
}

QString TeamRadarWindow::guessUserName() const
{
	QString result;

	// search environmental variables for user name
	QStringList envVariables;
	envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
				 << "HOSTNAME.*" << "DOMAINNAME.*";
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

void TeamRadarWindow::onSetImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose Image"), ".",
                                                    "Images (*.png *.jpg *.bmp *.ico)");
    if(fileName.isEmpty())
        return;

	// save photo file
	QPixmap pixmap = QPixmap(fileName).scaled(128, 128);
    ui.labelImage->setPixmap(pixmap);
	imageChanged = true;
	pixmap.save(userName + ".png");
}

void TeamRadarWindow::registerPhoto()
{
	QString photoPath = userName + ".png";
	QFile file(photoPath);
	if(!file.open(QFile::ReadOnly))
		return;

	QByteArray data = file.readAll();
	QByteArray format = QFileInfo(photoPath).suffix().toUtf8();
	Sender::getInstance()->sendPhotoRegistration(format, data);
}

void TeamRadarWindow::onUserListChanged() {
	resizeTable();
}

void TeamRadarWindow::onEditPeer(const QModelIndex& idx)
{
	QColor color = QColorDialog::getColor(
		model->data(model->index(idx.row(), PEER_COLOR)).toString());
	if(color.isValid())
	{
		model->setData(model->index(idx.row(), PEER_COLOR), color);
		model->submitAll();
		resizeTable();
	}
}

void TeamRadarWindow::resizeTable()
{
	ui.tvPeers->resizeRowsToContents();
	ui.tvPeers->resizeColumnsToContents();
}
