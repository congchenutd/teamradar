#include "ConnectionOptionWidget.h"
#include <QPixmap>
#include <Connection.h>

namespace TeamRadar {

ConnectionOptionWidget::ConnectionOptionWidget(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);

    // load settings
    setting = Setting::getInstance();
    ui.leServerAddress->setText (setting->getServerAddress());
    ui.sbPort         ->setValue(setting->getServerPort());
    ui.leUserName     ->setText (setting->getUserName());

    QPixmap pixmap = QPixmap(setting->getPhotoFilePath(setting->getUserName())).scaled(128, 128);   // photo
    if(!pixmap.isNull())
        ui.labelImage->setPixmap(pixmap);
    ui.labelColor->setColor(setting->getColor("DefaultDeveloperColor"));    // color
    ui.gridLayout->update();
}

void ConnectionOptionWidget::save()
{
    // save settings
    QString userName = ui.leUserName->text();
    setting->setServerAddress(ui.leServerAddress->text());
    setting->setServerPort(ui.sbPort->value());
    setting->setUserName(userName);
    Connection::getInstance()->setUserName(userName);

    QColor color = ui.labelColor->getColor();
    setting->setColor("DefaultDeveloperColor", color);

    QString photoPath = setting->getPhotoFilePath(userName);
    if(ui.labelImage->pixmap() != 0)
        ui.labelImage->pixmap()->save(photoPath);  // save photo file

    // save my photo and color info in the db
//	DeveloperInfo userInfo = model->getUserInfo(userName);
//	userInfo.image = photoPath;
//	userInfo.color = color;
//	PeerModel::updateUser(userInfo);

//	// send settings to the server
//	registerPhoto();
//	Sender::getInstance()->sendColorRegistration(color);

//	// update local view
//	PlayerWidget::getInstance()->setDeveloperImage(userName, QImage(photoPath));
//	PlayerWidget::getInstance()->setDeveloperColor(userName, color);
}

}
