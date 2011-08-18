#include "PeerModel.h"
#include "TeamRadarWindow.h"
#include "PeerManager.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include "Setting.h"

DeveloperInfo::DeveloperInfo(const QString& n) : name(n), online(false), receive(true)
{
	image = Setting::getInstance()->value("DefaultDeveloperImage").toString();
	color = Setting::getInstance()->getColor("DefaultDeveloperColor");
}

//////////////////////////////////////////////////////////////////////////
PeerModel::PeerModel(QObject *parent) : ImageColorBoolModel(parent) {}

bool PeerModel::select()
{
	bool result = ImageColorBoolModel::select();
	emit selected();
	return result;
}

//////////////////////////////////////////////////////////////////////////
bool PeerModel::openDB(const QString& name)
{
	QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
	database.setDatabaseName(name);
	if(!database.open())
	{
		QMessageBox::critical(0, "Error", QObject::tr("Can not open database %1").arg(name));
		return false;
	}
	return true;
}

void PeerModel::createTables()
{
	QSqlQuery query;
	query.exec("create table Peers( \
			   Name    varchar primary key, \
			   Color   varchar, \
			   Image   varchar, \
			   Online  bool, \
			   Receive bool \
			   )");
}

void PeerModel::makeAllOffline()
{
	QSqlQuery query;
	query.exec("update Peers set Online = \"false\"");
}

bool PeerModel::userExists(const QString& name)
{
	QSqlQuery query;
	query.exec(tr("select * from Peers where Name = \"%1\"").arg(name));
	return query.next();
}

void PeerModel::updateUser(const DeveloperInfo& info)
{
	QSqlQuery query;
	query.exec(tr("delete from Peers where Name = \"%1\"").arg(info.name));
	addUser(info);
}

void PeerModel::addUser(const DeveloperInfo& info)
{
	QSqlQuery query;
	query.exec(tr("insert into Peers values (\"%1\", \"%2\", \"%3\", \"%4\", \"%5\")")
		.arg(info.name).arg(info.color.name()).arg(info.image).arg(info.online).arg(info.receive));
}

DeveloperInfo PeerModel::getUserInfo(const QString& name)
{
	QSqlQuery query;
	query.exec(tr("select Color, Image, Online, Receive from Peers where Name =\"%1\"").arg(name));
	DeveloperInfo result(name);
	if(query.next())
	{
		result.color   = query.value(0).toString();
		result.image   = query.value(1).toString();
		result.online  = query.value(2).toBool();
		result.receive = query.value(3).toBool();
	}
	return result;
}

QPixmap PeerModel::toGrayPixmap(const QImage& colorImage)
{
	int width  = colorImage.width();
	int height = colorImage.height();
	QImage grayImage(width, height, QImage::Format_RGB32);
	for(int i=0; i<width; ++i)
		for(int j=0; j<height; ++j)
		{
			int pixel = colorImage.pixel(i, j);
			int alpha = qAlpha(pixel);
			if(alpha == 0)   // make transparent pixel white
				grayImage.setPixel(i, j, qRgb(255, 255, 255));
			else
			{
				int gray = qGray(pixel);
				grayImage.setPixel(i, j, qRgb(gray, gray, gray));
			}
		}
	return QPixmap::fromImage(grayImage);
}

bool PeerModel::isBlocked(const QString& name) {
	return !getUserInfo(name).receive;
}