#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QObject>
#include "TeamRadarEvent.h"
#include "Tag.h"

namespace TeamRadar {

struct TaggingEvent
{
	static TaggingEvent fromTeamRadarEvent(const TeamRadarEvent& trEvent);
	Tag toTag() const;
	bool isValid() const { return !tagName.isEmpty(); }

	QString userName;
	QString tagName;
	QString tagText;
	QString filePath;
	int     lineNumber;
};

class Communicator : public QObject
{
	Q_OBJECT
public:
	Communicator(QObject* parent = 0);
	void sendTaggingEvent(const Tag& tag);

signals:
	void remoteTagging(const TaggingEvent&);

private slots:
	void onTaggingEvent(const TeamRadarEvent& event);

};

}  // namespace TeamRadar

#endif // COMMUNICATOR_H
