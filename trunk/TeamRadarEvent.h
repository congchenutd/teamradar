#ifndef TEAMRADAREVENT_H
#define TEAMRADAREVENT_H

#include <QString>

struct TeamRadarEvent
{
	TeamRadarEvent::TeamRadarEvent(const QString& name, const QString& event, 
								   const QString& para = QString(), const QString& t = QString())
		: userName(name), eventType(event), parameters(para)	{
			time = t.isEmpty() ? QDateTime::currentDateTime() : QDateTime::fromString(t);
	}

	QString   userName;
	QString   eventType;
	QString   parameters;
	QDateTime time;
};


#endif // TEAMRADAREVENT_H
