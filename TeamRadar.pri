#include(EV.pri)
#include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)
#include($$IDE_SOURCE_TREE/src/plugins/coreplugin/coreplugin.pri)
#include($$IDE_SOURCE_TREE/src/plugins/projectexplorer/projectexplorer.pri)
#include($$IDE_SOURCE_TREE/src/plugins/vcsbase/vcsbase.pri)

#LIBS *= -l$$qtLibraryName(TeamRadar)
LIBRARY_NAME = TeamRadar
