#include "Communicator.h"
#include "Connection.h"
#include "Setting.h"

namespace TeamRadar {

Communicator::Communicator(QObject* parent) : QObject(parent)
{
	connect(Receiver::getInstance(), SIGNAL(newEvent(TeamRadarEvent)),
			this, SLOT(onTaggingEvent(TeamRadarEvent)));
}

void Communicator::sendTaggingEvent(const Tag& tag) {
	Sender::getInstance()->sendEvent("TAGGING", tag.toPacket());
}

void Communicator::onTaggingEvent(const TeamRadarEvent& event)
{
	TaggingEvent taggingEvent = TaggingEvent::fromTeamRadarEvent(event);
	if(taggingEvent.isValid())
		emit remoteTagging(taggingEvent);
}

///////////////////////////////////////////////////////////////////////////////
TaggingEvent TaggingEvent::fromTeamRadarEvent(const TeamRadarEvent& trEvent)
{
	TaggingEvent taggingEvent;
	if(trEvent.eventType != "TAGGING")
		return taggingEvent;

	// parameters = tagName;text;filename;linenumber
	QStringList sections = trEvent.parameters.split(Connection::Delimiter2);
	if(sections.size() <= 4)
		return taggingEvent;

	taggingEvent.userName   = trEvent.userName;
	taggingEvent.tagName    = sections[0];
	taggingEvent.tagText    = sections[1];
	taggingEvent.filePath   = sections[2];
	taggingEvent.lineNumber = sections[3].toInt();
	return taggingEvent;
}

Tag TaggingEvent::toTag() const
{
	Tag tag;
	TagKeywords keywords = Setting::getInstance()->getTags();
	if(keywords.contains(tagName))
	{
		tag.keyword    = keywords[tagName];
		tag.text       = tagText;
		tag.filePath   = filePath;
		tag.lineNumber = lineNumber;
	}
	return tag;
}

}  // namespace TeamRadar
