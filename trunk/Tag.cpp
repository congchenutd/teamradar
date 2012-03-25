#include "Tag.h"
#include "Connection.h"

namespace TeamRadar {

QDataStream& operator<<(QDataStream& os, const TagKeyword& tag)
{
	os << tag.name;
	os << tag.icon;
	os << tag.bgColor;
	return os;
}

QDataStream& operator>>(QDataStream& is, TagKeyword& tag)
{
	is >> tag.name;
	is >> tag.icon;
	is >> tag.bgColor;
	return is;
}

/////////////////////////////////////////////////////////////////
QString Tag::toString() const
{
	QStringList sections;
	sections << userName << keyword.name << text << filePath << QString::number(lineNumber);
	return sections.join(": ");
}

QString Tag::toPacket() const
{
	QStringList sections;
	sections << keyword.name << text << filePath << QString::number(lineNumber);
	return sections.join(QString(Connection::Delimiter2));
}


}  // namespace TeamRadar
