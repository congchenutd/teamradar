#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QObject>
#include "TeamRadarEvent.h"

namespace TeamRadar {

struct TaggingEvent
{
	QString userName;
	QString tag;
	QString fileName;
	int     lineNumber;
};

class Communicator : public QObject
{
	Q_OBJECT
public:
	Communicator(QObject* parent = 0);
	void sendTaggingEvent(const QString& text, const QString& filePath, int row);

signals:
	void remoteTagging(const TaggingEvent&);

private slots:
	void onTaggingEvent(const TeamRadar::TeamRadarEvent& event);

};

}  // namespace TeamRadar

#endif // COMMUNICATOR_H
