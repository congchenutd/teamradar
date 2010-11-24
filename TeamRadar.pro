TEMPLATE = lib
TARGET = TeamRadar

include(../../qtcreatorplugin.pri)
include(../../plugins/coreplugin/coreplugin.pri)

QT += network

HEADERS += TeamRadarPlugin.h \
	TeamRadarWindow.h \
	MessageCollector.h \
	TeamRadarView.h \
	Connection.h

SOURCES += TeamRadarPlugin.cpp \
	TeamRadarWindow.cpp \
	MessageCollector.cpp \
	TeamRadarView.cpp \
	Connection.cpp

FORMS += TeamRadarDlg.ui \
	 TeamRadarView.ui

OTHER_FILES += TeamRadar.pluginspec
