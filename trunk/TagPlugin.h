#ifndef TODOPLUGIN_H
#define TODOPLUGIN_H
#include <extensionsystem/iplugin.h>

namespace TeamRadar {

class TagPlugin : public ExtensionSystem::IPlugin
{
public:
	virtual void extensionsInitialized() {}
	virtual bool initialize(const QStringList& arguments, QString* errorString);
};

}  // namespace TeamRadar

#endif // TODOPLUGIN_H

