#-------------------------------------------------
#
# Project created by QtCreator 2017-06-06T06:31:52
#
#-------------------------------------------------

QT += core gui charts concurrent network widgets
QT += sql qml webenginewidgets uitools
QT += webengine webchannel 3dcore 3drender 3dextras

#MOC_DIR = $$DESTDIR/.moc
#UI_DIR = $$DESTDIR/.ui
#OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$OUT_PWD/.moc
UI_DIR = $$OUT_PWD/.ui
OBJECTS_DIR = $$OUT_PWD/.obj

CONFIG += c++17

TARGET = PBE
TEMPLATE = app

VERSION=2.2.3
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

include($$PWD/ConanHelper.pri)

win32{
    LIBS = $$replace(LIBS, .dll.lib, .dll)
    LIBS += -lAdvapi32
    LIBS +=CRYPT32.lib
    LIBS +=Ws2_32.lib
    LIBS +=User32.lib
    DEFINES += CURL_STATICLIB
}

win32 {
    RC_ICONS = icons/NHERI-PBE-Icon.ico
} else {
    mac {
      ICON = icons/NHERI-PBE-Icon.icns
      DEFINES += _GRAPHICS_Qt3D
      QMAKE_INFO_PLIST=$$PWD/Info.plist
    } else {
      LIBS += -lc
    }
}

include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/Workflow/Workflow.pri)
include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/InputSheetBM/InputSheetBM.pri)
include(../EE-UQ/EarthquakeEvents.pri)
include(../QS3hark/QS3hark.pri)

INCLUDEPATH += "./Component"

SOURCES += main.cpp \
    HDF5Handler.cpp \
    WorkflowAppPBE.cpp \
    RunWidget.cpp \
    ResultsPelicun.cpp \
    LossModel/P58LossModel.cpp \
    LossModel/P58GeneralSettingsContainer.cpp \
    LossModel/P58ComponentContainer.cpp \
    LossModel/P58CollapseModeContainer.cpp \
    LossModel/P58DependenciesContainer.cpp \
    LossModel/P58CollapseMode.cpp \
    LossModel/LossModelSelection.cpp \
    LossModel/HazusGeneralSettingsContainer.cpp \
    LossModel/HazusLossModel.cpp \
    LossModel/LossMethod.cpp \
    LossModel/P58ComponentGroup.cpp


HEADERS  += \
    HDF5Handler.h \
    WorkflowAppPBE.h \
    RunWidget.h \
    ResultsPelicun.h \
    LossModel/P58LossModel.h \
    LossModel/P58GeneralSettingsContainer.h \
    LossModel/P58ComponentContainer.h \
    LossModel/P58CollapseModeContainer.h \
    LossModel/P58DependenciesContainer.h \
    LossModel/P58CollapseMode.h \
    LossModel/LossModelSelection.h \
    LossModel/HazusLossModel.h \
    LossModel/HazusGeneralSettingsContainer.h \
    LossModel/LossMethod.h \
    LossModel/P58ComponentGroup.h


RESOURCES += \
    images.qrc 

OTHER_FILES += conanfile.py

