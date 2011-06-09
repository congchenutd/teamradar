#include "TeamRadarWindow.h"
#include "Connection.h"

#include <QHostAddress>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QProcess>
#include <QFileDialog>

using namespace TeamRadar;

TeamRadarWindow::TeamRadarWindow(QWidget *parent) : QDialog(parent)
{
	imageChanged = false;
    ui.setupUi(this);

	setting = MySetting<UserSetting>::getInstance();
	ui.leServerAddress->setText(setting->getServerAddress());
	ui.sbPort->setValue(setting->getServerPort());
	QString userName = setting->getUserName();
	if(userName.isEmpty())
		userName = guessUserName();
	ui.leUserName->setText(userName);

    connect(ui.btImage, SIGNAL(clicked()), this, SLOT(onSetImage()));
}

void TeamRadar::TeamRadarWindow::accept()
{
	setting->setServerAddress(ui.leServerAddress->text());
	setting->setServerPort(ui.sbPort->value());
	setting->setUserName(ui.leUserName->text());
	QDialog::accept();
}

QString TeamRadar::TeamRadarWindow::guessUserName() const
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

	if(result.isEmpty())
		result = "Unknown";
	return result;
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
	pixmap.save("Photo.png");

	// register the photo on server
	Connection::getInstance()->registerPhoto("Photo.png");
}


/////////////////////////////////////// UserSetting ////////////////////////////////
TeamRadar::UserSetting::UserSetting(const QString& fileName) : MySetting<UserSetting>(fileName)
{
	if(QFile(this->fileName).size() == 0)   // no setting
		loadDefaults();
}

void TeamRadar::UserSetting::loadDefaults()
{
	setServerAddress("0.0.0.0");
	setServerPort(12345);
	setUserName("Unknown");
    setValue("Image", "");
}

QString TeamRadar::UserSetting::getServerAddress() const {
	return value("ServerAddress").toString();
}

quint16 TeamRadar::UserSetting::getServerPort() const {
	return value("ServerPort").toInt();
}

void TeamRadar::UserSetting::setServerAddress(const QString& address) {
	setValue("ServerAddress", address);
}

void TeamRadar::UserSetting::setServerPort(quint16 port) {
	setValue("ServerPort", port);
}

QString TeamRadar::UserSetting::getUserName() const {
	return value("UserName").toString();
}

void TeamRadar::UserSetting::setUserName(const QString& name) {
	setValue("UserName", name);
}
