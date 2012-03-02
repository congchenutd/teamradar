TEMPLATE = lib
TARGET = TeamRadar
PROVIDER = CongChen
QT += sql network

include(EV.pri)
include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)
include($$IDE_SOURCE_TREE/src/plugins/coreplugin/coreplugin.pri)
include($$IDE_SOURCE_TREE/src/plugins/projectexplorer/projectexplorer.pri)
include($$IDE_SOURCE_TREE/src/plugins/vcsbase/vcsbase.pri)

win32 {
LIBS += -L$$IDE_LIBRARY_PATH \
		-L$$IDE_PLUGIN_PATH/Nokia
}
macx {
LIBS += -L$$IDE_BUILD_TREE/bin/QtCreator.app/Contents/Plugins \
		-L$$IDE_BUILD_TREE/bin/QtCreator.app/Contents/PlugIns/Nokia
}


INCLUDEPATH += ../ImageColorBoolModel

# Input
HEADERS += ChatWindow.h \
		   Connection.h \
		   Edge.h \
		   Engine.h \
		   Label.h \
		   LightTrail.h \
		   MessageCollector.h \
		   ModeTag.h \
		   MySetting.h \
		   Node.h \
		   PeerManager.h \
		   PeerModel.h \
		   PlayerWidget.h \
		   RecipientsDlg.h \
		   RequestEventsDlg.h \
		   Setting.h \
		   TeamRadarEvent.h \
		   TeamRadarPlugin.h \
		   TeamRadarView.h \
		   TeamRadarDlg.h \
		   Utility.h \
		   ../ImageColorBoolModel/ImageColorBoolDelegate.h \
		   ../ImageColorBoolModel/ImageColorBoolProxy.h \
		   OptionsPage.h \
		   ImageLabel.h \
		   Analyzer.h \
		   Defines.h \
		   ColorLabel.h
FORMS += ChatWindow.ui \
		 PlayerWidget.ui \
		 RequestEventsDlg.ui \
		 TeamRadarDlg.ui \
		 Analyzer.ui
SOURCES += ChatWindow.cpp \
		   Connection.cpp \
		   Edge.cpp \
		   Engine.cpp \
		   Label.cpp \
		   LightTrail.cpp \
		   MessageCollector.cpp \
		   ModeTag.cpp \
		   Node.cpp \
		   PeerManager.cpp \
		   PeerModel.cpp \
		   PlayerWidget.cpp \
		   RecipientsDlg.cpp \
		   RequestEventsDlg.cpp \
		   Setting.cpp \
		   TeamRadarPlugin.cpp \
		   TeamRadarView.cpp \
		   TeamRadarDlg.cpp \
		   Utility.cpp \
		   ../ImageColorBoolModel/ImageColorBoolDelegate.cpp \
		   ../ImageColorBoolModel/ImageColorBoolProxy.cpp \
		   OptionsPage.cpp \
		   ImageLabel.cpp \
		   Analyzer.cpp \
		   ColorLabel.cpp
RESOURCES += Resources.qrc
OTHER_FILES += TeamRadar.pluginspec
