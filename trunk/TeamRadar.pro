TEMPLATE = lib
TARGET = TeamRadar
QT += sql network
PROVIDER = CongChen
#DEFINES += TEAMRADAR_LIBRARY QT_NO_CAST_FROM_ASCII

include(TeamRadar_dependencies.pri)

LIBS += -L$$IDE_LIBRARY_PATH \
		-L$$IDE_PLUGIN_PATH/Nokia

INCLUDEPATH += ../ImageColorBoolModel

# Input
HEADERS += ChatWindow.h \
		   Connection.h \
		   Edge.h \
		   Engine.h \
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
		   ImageLabel.h \
		   Analyzer.h \
		   Defines.h \
		   ColorLabel.h \
    TeamRadarOptionsPage.h \
    NodeLabel.h \
    TagOutputPane.h \
    TagOptionsPage.h \
    TagOptionsDlg.h \
    TagKeyword.h \
    TaggingManager.h \
    TagDlg.h \
    Communicator.h
FORMS += ChatWindow.ui \
		 PlayerWidget.ui \
		 RequestEventsDlg.ui \
		 TeamRadarDlg.ui \
		 Analyzer.ui \
    TagOptionsDlg.ui \
    TagDlg.ui
SOURCES += ChatWindow.cpp \
		   Connection.cpp \
		   Edge.cpp \
		   Engine.cpp \
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
		   ImageLabel.cpp \
		   Analyzer.cpp \
		   ColorLabel.cpp \
    TeamRadarOptionsPage.cpp \
    NodeLabel.cpp \
    TagOutputPane.cpp \
    TagOptionsPage.cpp \
    TagOptionsDlg.cpp \
    TagKeyword.cpp \
    TaggingManager.cpp \
    TagDlg.cpp \
    Communicator.cpp
RESOURCES += Resources.qrc
OTHER_FILES += TeamRadar.pluginspec
