#ifndef Setting_h__
#define Setting_h__

#include "MySetting.h"

class Setting : public MySetting<Setting>
{
public:
	Setting(const QString& fileName);
	QColor  getColor(const QString& section) const;
	QColor  getExtensionColor(const QString& fileName) const;
	QString getServerAddress() const;
	quint16 getServerPort()    const;
	QString getUserName()      const;
	QString getPhotoFilePath(const QString& userName) const;
	QString getChatHistoryPath() const;
	QString getRootPath() const;
	qreal   getThreshold() const;
	int     getFontSize() const;

	void setServerAddress(const QString& address);
	void setServerPort(quint16 port);
	void setUserName(const QString& name);
	void setColor(const QString& section, const QColor& color);
	void setExtensionColor(const QString& extension, const QColor& color);
	void setRootPath(const QString& path);
	void setThreshold(qreal sensitivity);
	void setFontSize(int size);

private:
    void loadDefaults();

public:
	static const QString dateTimeFormat;
};

#endif // Setting_h__
