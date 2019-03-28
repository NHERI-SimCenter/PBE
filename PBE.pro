#-------------------------------------------------
#
# Project created by QtCreator 2017-06-06T06:31:52
#
#-------------------------------------------------

QT       += core gui charts concurrent network sql qml webenginewidgets uitools webengine webchannel

TARGET = PBE
TEMPLATE = app

macos:LIBS += /usr/lib/libcurl.dylib
win32:INCLUDEPATH += "c:\Users\SimCenter\libCurl-7.59.0\include"
win32:LIBS += C:\Users\SimCenter\libCurl-7.59.0/lib/libcurl.lib
linux:LIBS += /usr/lib/x86_64-linux-gnu/libcurl.so

win32 {
    RC_ICONS = icons/NHERI-PBE-Icon.ico
} else {
    mac {
    ICON = icons/NHERI-PBE-Icon.icns
    }
}

include(../EE-UQ/EE-UQ.pri)
include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/InputSheetBM/InputSheetBM.pri)
include(../GroundMotionUtilities/UI/GroundMotionWidgets.pri)
include(../SiteResponseTool/SiteResponseTool.pri)
include(./MiniZip/MiniZip.pri)

INCLUDEPATH += "./Component"

SOURCES += main.cpp \
    WorkflowAppPBE.cpp \
    RunWidget.cpp \
    LossModel/Component.cpp \
    LossModel/ComponentContainer.cpp \
    LossModel/CollapseMode.cpp \
    LossModel/CollapseModeContainer.cpp \
    LossModel/GeneralSettingsContainer.cpp \
    LossModel/LossModelContainer.cpp \
    ResultsPelicun.cpp


HEADERS  += \
    WorkflowAppPBE.h \
    RunWidget.h \
    LossModel/Component.h \
    LossModel/ComponentContainer.h \
    LossModel/CollapseMode.h \
    LossModel/CollapseModeContainer.h \
    LossModel/GeneralSettingsContainer.h \
    LossModel/LossModelContainer.h \
    ResultsPelicun.h


RESOURCES += \
    #resources.qrc
    #../EE-UQ/images.qrc \


#FORMS    += mainwindow.ui

#RESOURCES += \
#    schema.qrc



