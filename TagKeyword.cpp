#include "TagKeyword.h"

namespace TeamRadar {

QDataStream& operator<<(QDataStream& os, const TagKeyword& tag)
{
	os << tag.name;
	os << tag.icon;
	os << tag.bgColor;
	return os;
}

QDataStream& operator>>(QDataStream& is,  TagKeyword& tag)
{
	is >> tag.name;
	is >> tag.icon;
	is >> tag.bgColor;
	return is;
}

}  // namespace TeamRadar
