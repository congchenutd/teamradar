#include "PeerModel.h"
#include "TeamRadarWindow.h"

PeerModel::PeerModel(QObject *parent) : QStandardItemModel(parent) {}

QVariant PeerModel::data(const QModelIndex& idx, int role) const
{
	if(!idx.isValid())
		return QStandardItemModel::data(idx, role);

	if(idx.column() == TeamRadarWindow::PEER_NAME)        // name
	{
		if(role == Qt::DecorationRole)
		{
			QPixmap px(data(
				index(idx.row(), TeamRadarWindow::PEER_IMAGE), Qt::DisplayRole).toString());
			return px.scaled(32, 32);
		}
	}
	else if(idx.column() == TeamRadarWindow::PEER_COLOR)   // Color
	{
		if(role == Qt::DecorationRole)
			return QColor(data(idx, Qt::DisplayRole).toString());
		else if(role == Qt::DisplayRole)
			return QVariant();
	}

	return QStandardItemModel::data(idx, role);
}