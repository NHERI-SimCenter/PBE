#-------------------------------------------------
#
# Project created by QtCreator 2017-06-06T06:31:52
#
#-------------------------------------------------

QT       += core gui charts concurrent network sql qml webenginewidgets uitools webengine webchannel 3dcore 3drender 3dextras

TARGET = PBE
TEMPLATE = app

macos:LIBS += /usr/lib/libcurl.dylib -llapack -lblas
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

include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/Workflow/Workflow.pri)
include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/InputSheetBM/InputSheetBM.pri)
include(../EE-UQ/EarthquakeEvents.pri)
include(../GroundMotionUtilities/UI/GroundMotionWidgets.pri)
include(../s3hark/s3hark.pri)
include(./MiniZip/MiniZip.pri)

INCLUDEPATH += "./Component"

SOURCES += main.cpp \
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



#FORMS    += mainwindow.ui

#RESOURCES += \
#    schema.qrc



