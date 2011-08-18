#ifndef PEER_MODEL_H
#define PEER_MODEL_H

#include "ImageColorBoolModel.h"

struct DeveloperInfo
{
	DeveloperInfo(const QString& name);
	QString name;
	QColor  color;
	bool    online; 
	QString image;
	bool    receive;
};

class PeerModel : public ImageColorBoolModel
{
	Q_OBJECT

public:
	PeerModel(QObject* parent = 0);
	bool select();
	
	static bool openDB(const QString& dbName);
	static void createTables();
	static void makeAllOffline();
	static bool userExists(const QString& name);
	static void updateUser(const DeveloperInfo& info);
	static void addUser   (const DeveloperInfo& info);
	static DeveloperInfo getUserInfo(const QString& name);
	static bool isBlocked(const QString& name);

private:
	static QPixmap toGrayPixmap(const QImage& colorImage);

signals:
	void selected();

public:
	enum {NAME, COLOR, IMAGE, ONLINE, RECEIVE};
};


class Test : public QObject
{
Q_OBJECT
signals:
	void foo();
};

#endif