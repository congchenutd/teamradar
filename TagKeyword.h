#ifndef TAG_H
#define TAG_H

#include <QIcon>
#include <QColor>
#include <QString>
#include <QList>
#include <QMetaType>

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

typedef QList<TagKeyword> TagKeywords;

QDataStream& operator<<(QDataStream& out, const TagKeyword& tag);
QDataStream& operator>>(QDataStream& in,  TagKeyword& tag);

Q_DECLARE_METATYPE(TagKeywords)
Q_DECLARE_METATYPE(TagKeyword)

}  // namespace TeamRadar

#endif // TAG_H
