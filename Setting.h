#ifndef Setting_h__
#define Setting_h__

#include "MySetting.h"
#include "Tag.h"

namespace TeamRadar {

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
	bool    showLightTrail() const;
	bool    showAfterImage() const;
	int     getEngineRate() const;
	int     getEngineSubtlety() const;
	QString getCompileDate() const;

	void setServerAddress(const QString& address);
	void setServerPort(quint16 port);
	void setUserName(const QString& name);
	void setColor(const QString& section, const QColor& color);
	void setExtensionColor(const QString& extension, const QColor& color);
	void setRootPath(const QString& path);
	void setFontSize(int size);
	void setShowLightTrail(bool show);
	void setShowAfterImage(bool show);
	void setEngineSubtlety(int subtlety);

	// for tag
	void setTags(const TagKeywords& tags);
	TagKeywords getTags() const;
	TagKeywords getDefaultTags() const;

private:
	void loadDefaults();
	QString guessUserName() const;

public:
	static const QString dateTimeFormat;
};

} // namespace TeamRadar

#endif // Setting_h__
