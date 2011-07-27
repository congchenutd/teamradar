#ifndef PEER_MODEL_H
#define PEER_MODEL_H

#include <QStandardItemModel>

class PeerModel : public QStandardItemModel
{
public:
	PeerModel(QObject* parent = 0);
	QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const;
};

#endif