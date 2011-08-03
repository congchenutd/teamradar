#include "PeerModel.h"
#include "TeamRadarWindow.h"
#include "PeerManager.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>

PeerModel::PeerModel(QObject *parent) : QSqlTableModel(parent) {}

QVariant PeerModel::data(const QModelIndex& idx, int role) const
{
	if(!idx.isValid())
		return QSqlTableModel::data(idx, role);

	if(idx.column() == TeamRadarWindow::PEER_NAME)        // name
	{
		if(role == Qt::DecorationRole)
		{
			QImage image(data(
				index(idx.row(), TeamRadarWindow::PEER_IMAGE)).toString());
			image = image.scaled(64, 64);
			bool online = data(index(idx.row(), TeamRadarWindow::PEER_ONLINE)).toBool();
			QPixmap pixmap = online ? QPixmap::fromImage(image) : toGrayPixmap(image);
			return pixmap;
		}
	}
	else if(idx.column() == TeamRadarWindow::PEER_COLOR)   // Color
	{
		if(role == Qt::DecorationRole)
			return QColor(data(idx, Qt::DisplayRole).toString());
		else if(role == Qt::DisplayRole)
			return QVariant();
	}

	return QSqlTableModel::data(idx, role);
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
			   Name   varchar primary key, \
			   Color  varchar, \
			   Image  varchar, \
			   Online bool \
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
	query.exec(tr("update Peers set Image  = \"%1\"   where Name = \"%2\"").arg(info.image).arg(info.name));
	query.exec(tr("update Peers set Online = \"true\" where Name = \"%1\"").arg(info.name));
}

QPixmap PeerModel::toGrayPixmap(const QImage& colorImage)
{
	int width  = colorImage.width();
	int height = colorImage.height();
	QImage grayImage(width, height, QImage::Format_RGB32);
	for(int i=0; i<width; ++i)
		for(int j=0; j<height; ++j)
		{
			int gray = qGray(colorImage.pixel(i, j));
			grayImage.setPixel(i, j, qRgb(gray, gray, gray));
		}
	return QPixmap::fromImage(grayImage);
}