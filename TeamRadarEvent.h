#ifndef TEAMRADAREVENT_H
#define TEAMRADAREVENT_H

#include <QString>

struct TeamRadarEvent
{
	TeamRadarEvent::TeamRadarEvent(const QString& name, const QString& event, const QString& para = QString())
		: userName(name), eventType(event), parameter(para)
	{}

	QString userName;
	QString eventType;
	QString parameter;
};

#endif // TEAMRADAREVENT_H
