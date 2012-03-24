#include "Communicator.h"
#include "Connection.h"

namespace TeamRadar {

Communicator::Communicator(QObject* parent) : QObject(parent)
{
	connect(Receiver::getInstance(), SIGNAL(newEvent(TeamRadarEvent)),
			this, SLOT(onTaggingEvent(TeamRadarEvent)));
}

void Communicator::sendTaggingEvent(const QString& text, const QString& filePath, int row)
{
	Sender::getInstance()->sendEvent("TAGGING", text + ": " + filePath + ": " + QString::number(row));
}

void Communicator::onTaggingEvent(const TeamRadar::TeamRadarEvent& event)
{
	if(event.eventType != "TAGGING")
		return;

	// parameters = tag;filename;linenumber
	QStringList sections = event.parameters.split(TeamRadar::Connection::Delimiter1);
	if(sections.size() <= 3)
		return;

	TaggingEvent taggingEvent;
	taggingEvent.userName   = event.userName;
	taggingEvent.tag        = sections[0];
	taggingEvent.fileName   = sections[1];
	taggingEvent.lineNumber = sections[2].toInt();
	emit remoteTagging(taggingEvent);
}

}  // namespace TeamRadar
