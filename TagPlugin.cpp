#include "TagPlugin.h"
#include "TagOptionsPage.h"
#include "TaggingManager.h"
#include "TagOutputPane.h"
#include <QtPlugin>

namespace TeamRadar {

bool TagPlugin::initialize(const QStringList& args, QString* errMsg)
{
	Q_UNUSED(args);
	Q_UNUSED(errMsg);

	qRegisterMetaTypeStreamOperators<TagKeyword> ("TagKeyword");
	qRegisterMetaTypeStreamOperators<TagKeywords>("TagKeywords");

	TagOutputPane* outPane = new TagOutputPane(this);
	new TaggingManager(outPane, this);

	addAutoReleasedObject(outPane);
	addAutoReleasedObject(new TagOptionsPage(this));
	addAutoReleasedObject(new TagAboutPage(this));

	return true;
}

}  // namespace TeamRadar

Q_EXPORT_PLUGIN(TeamRadar::TagPlugin)


