#ifndef Setting_h__
#define Setting_h__

#include "MySetting.h"

class Setting : public MySetting<Setting>
{
public:
	Setting(const QString& fileName);
	QColor getColor(const QString& section) const;
	void   setColor(const QString& section, const QColor& color);
	QColor getExtensionColor(const QString& fileName) const;
	void   setExtensionColor(const QString& extension, const QColor& color);

	QString getServerAddress() const;
	quint16 getServerPort()    const;
	QString getUserName()      const;
	QString getPhotoFilePath(const QString& userName) const;
	QString getChatHistoryPath() const;

	void setServerAddress(const QString& address);
	void setServerPort(quint16 port);
	void setUserName(const QString& name);

private:
    void loadDefaults();
};

#endif // Setting_h__
