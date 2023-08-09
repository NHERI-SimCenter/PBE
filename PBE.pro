#-------------------------------------------------
#
# Project created by QtCreator 2017-06-06T06:31:52
#
#-------------------------------------------------

QT += core gui charts concurrent network widgets
QT += sql qml webenginewidgets core5compat
QT += webchannel 3dcore 3drender 3dextras

#MOC_DIR = $$DESTDIR/.moc
#UI_DIR = $$DESTDIR/.ui
#OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$OUT_PWD/.moc
UI_DIR = $$OUT_PWD/.ui
OBJECTS_DIR = $$OUT_PWD/.obj

CONFIG += c++17

TARGET = PBE
TEMPLATE = app

VERSION=3.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

#include($$PWD/ConanHelper.pri)

win32{
    LIBS = $$replace(LIBS, .dll.lib, .dll)
    LIBS += -lAdvapi32
    LIBS +=CRYPT32.lib
    LIBS +=Ws2_32.lib
    LIBS +=User32.lib
    DEFINES += CURL_STATICLIB
}


LIBS += -L/opt/homebrew/Cellar/curl/8.0.1/lib/ -lcurl.4
INCLUDEPATH += /opt/homebrew/Cellar/curl/8.1.2_1/include
INCLUDEPATH += /opt/homebrew/Cellar/quazip/1.4/include/quazip

macx: LIBS += -L/opt/homebrew/Cellar/quazip/1.4/lib/ -lquazip1-qt6.1.4
INCLUDEPATH += $$/opt/homebrew/Cellar/quazip/1.4/include

                   
include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/Workflow/Workflow.pri)
include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/InputSheetBM/InputSheetBM.pri)
include(../EE-UQ/EarthquakeEvents.pri)
include(../QS3hark/QS3hark.pri)

INCLUDEPATH += "./Component" \
                PerformanceMethod

SOURCES += main.cpp \
    WorkflowAppPBE.cpp \
    RunWidget.cpp \
    ResultsPelicun.cpp \
    LossModel/PelicunLossModel.cpp \
    LossModel/PelicunPreferencesContainer.cpp \
    LossModel/PelicunComponentContainer.cpp \
    LossModel/PelicunDemandContainer.cpp \
    LossModel/PelicunDamageContainer.cpp \
    LossModel/PelicunLossContainer.cpp \
    LossModel/PelicunLossRepairContainer.cpp \
    LossModel/PelicunCollapseModeContainer.cpp \
    LossModel/PelicunCollapseLimit.cpp \
    LossModel/PelicunTruncationLimit.cpp \
    LossModel/PelicunResidualParams.cpp \
    LossModel/PelicunOutputsContainer.cpp \
    LossModel/PelicunCollapseMode.cpp \
    LossModel/PelicunComponentGroup.cpp \
    LossModel/PelicunPopulationGroup.cpp \
    LossModel/LossModelSelection.cpp \
    LossModel/HazusGeneralSettingsContainer.cpp \
    LossModel/HazusLossModel.cpp \
    LossModel/LossMethod.cpp \
    LossModel/PelicunShared.cpp \
    PerformanceMethod/PerformanceMethodSelection.cpp \
    PerformanceMethod/REDiWidget.cpp \



HEADERS  += \
    WorkflowAppPBE.h \
    RunWidget.h \
    ResultsPelicun.h \
    LossModel/PelicunLossModel.h \
    LossModel/PelicunPreferencesContainer.h \
    LossModel/PelicunComponentContainer.h \
    LossModel/PelicunDemandContainer.h \
    LossModel/PelicunDamageContainer.h \
    LossModel/PelicunLossContainer.h \
    LossModel/PelicunLossRepairContainer.h \
    LossModel/PelicunCollapseModeContainer.h \
    LossModel/PelicunCollapseLimit.h \
    LossModel/PelicunTruncationLimit.h \
    LossModel/PelicunResidualParams.h \
    LossModel/PelicunOutputsContainer.h \
    LossModel/PelicunCollapseMode.h \
    LossModel/PelicunComponentGroup.h \
    LossModel/PelicunPopulationGroup.h \
    LossModel/LossModelSelection.h \
    LossModel/HazusLossModel.h \
    LossModel/HazusGeneralSettingsContainer.h \
    LossModel/LossMethod.h \
    LossModel/PelicunShared.h \
    PerformanceMethod/PerformanceMethodSelection.h \
    PerformanceMethod/REDiWidget.h \

RESOURCES += \
    images.qrc

#OTHER_FILES += conanfile.py
