#ifndef TAG_H
#define TAG_H

#include <QIcon>
#include <QColor>
#include <QString>
#include <QMap>
#include <QMetaType>
#include <QFileInfo>

namespace TeamRadar {

class TagKeyword
{
public:
	TagKeyword() {}
	TagKeyword(QString n, QIcon i, QColor c) : name(n), icon(i), bgColor(c) {}
	bool isValid() const { return !name.isEmpty(); }

	QString name;
	QIcon   icon;
	QColor  bgColor;
};

typedef QMap<QString, TagKeyword> TagKeywords;

QDataStream& operator<<(QDataStream& out, const TagKeyword& tag);
QDataStream& operator>>(QDataStream& in,  TagKeyword& tag);


//////////////////////////////////////////////////////////////////////
class Tag
{
public:
	Tag() {}
	QString toString() const;
	QString toPacket() const;    // for network transmisson
	bool    isValid() const { return keyword.isValid(); }

	QString    userName;
	TagKeyword keyword;
	QString    text;
	QString    filePath;
	int        lineNumber;
};

}  // namespace TeamRadar

Q_DECLARE_METATYPE(TeamRadar::TagKeywords)
Q_DECLARE_METATYPE(TeamRadar::TagKeyword)

#endif // TAG_H
