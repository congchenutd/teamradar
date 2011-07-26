#include "TeamRadarWindow.h"
#include "Connection.h"
#include "Setting.h"
#include <QHostAddress>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QProcess>
#include <QFileDialog>

TeamRadarWindow::TeamRadarWindow(QWidget *parent) : QDialog(parent)
{
	imageChanged = false;
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

    connect(ui.btImage, SIGNAL(clicked()), this, SLOT(onSetImage()));
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
	pixmap.save(userName + ".png");
}

void TeamRadarWindow::registerPhoto()
{
	Connection* connection = Connection::getInstance();
	if(connection->getState() != Connection::ReadyForUse)
		return;

	QString photoPath = userName + ".png";
	QFile file(photoPath);
	if(!file.open(QFile::ReadOnly))
		return;

	QByteArray data = file.readAll();
	QString format = QFileInfo(photoPath).suffix();
	connection->write("REGISTER_PHOTO#" + 
		               QByteArray::number(data.size() + format.length()) + "#" + 
					   format.toUtf8() + "#" + data);
}