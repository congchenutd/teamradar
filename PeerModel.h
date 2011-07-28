#ifndef PEER_MODEL_H
#define PEER_MODEL_H

#include <QSqlTableModel>

struct DeveloperInfo;

class PeerModel : public QSqlTableModel
{
public:
	PeerModel(QObject* parent = 0);
	QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const;
	
	static bool openDB(const QString& dbName);
	static void createTables();
	static void makeAllOffline();
	static bool userExists(const QString& name);
	static void updateUser(const DeveloperInfo& info);
};

#endif