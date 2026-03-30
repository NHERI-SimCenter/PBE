cmake_minimum_required(VERSION 3.21)

project(PBE VERSION 1.0.0 LANGUAGES C CXX)

# ---- C++17 ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Qt ----
find_package(Qt6 REQUIRED COMPONENTS
    Core Widgets Gui Charts Concurrent Network PrintSupport 
    WebEngineWidgets WebEngineCore 3DCore 3DRender 3DExtras
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ---- Conan Packages -----
find_package(ZLIB REQUIRED)
find_package(CURL REQUIRED)

# ---- QuaZip -----
find_package(QuaZip-Qt6 REQUIRED)

# ---- Resources, Forms, and Icons ----

set(RESOURCES
    styles.qrc
    images.qrc
)

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE NHERI-PBE-Icon.icns)
    set(APP_ICON icons/NHERI-PBE-Icon.icns)
    set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
elseif(WIN32)
    set(APP_ICON icons/NHERI-PBE-Icon.ico)
endif()

# ---- Sources and Headers ----

set(SOURCES main.cpp
    WorkflowAppPBE.cpp
    RunWidget.cpp
    ResultsPelicun.cpp
    LossModel/PelicunLossModel.cpp
    LossModel/PelicunPreferencesContainer.cpp
    LossModel/PelicunComponentContainer.cpp
    LossModel/PelicunDemandContainer.cpp
    LossModel/PelicunDamageContainer.cpp
    LossModel/PelicunLossContainer.cpp
    LossModel/PelicunLossRepairContainer.cpp
    LossModel/PelicunCollapseModeContainer.cpp
    LossModel/PelicunCollapseLimit.cpp
    LossModel/PelicunTruncationLimit.cpp
    LossModel/PelicunResidualParams.cpp
    LossModel/PelicunOutputsContainer.cpp
    LossModel/PelicunCollapseMode.cpp
    LossModel/PelicunComponentGroup.cpp
    LossModel/PelicunPopulationGroup.cpp
    LossModel/LossModelSelection.cpp
    LossModel/HazusGeneralSettingsContainer.cpp
    LossModel/HazusLossModel.cpp
    LossModel/LossMethod.cpp
    LossModel/PelicunShared.cpp
    PerformanceMethod/PerformanceMethodSelection.cpp
    PerformanceMethod/REDiWidget.cpp
)

set(HEADERS
    WorkflowAppPBE.h
    RunWidget.h
    ResultsPelicun.h
    LossModel/PelicunLossModel.h
    LossModel/PelicunPreferencesContainer.h
    LossModel/PelicunComponentContainer.h
    LossModel/PelicunDemandContainer.h
    LossModel/PelicunDamageContainer.h
    LossModel/PelicunLossContainer.h
    LossModel/PelicunLossRepairContainer.h
    LossModel/PelicunCollapseModeContainer.h
    LossModel/PelicunCollapseLimit.h
    LossModel/PelicunTruncationLimit.h
    LossModel/PelicunResidualParams.h
    LossModel/PelicunOutputsContainer.h
    LossModel/PelicunCollapseMode.h
    LossModel/PelicunComponentGroup.h
    LossModel/PelicunPopulationGroup.h
    LossModel/LossModelSelection.h
    LossModel/HazusLossModel.h
    LossModel/HazusGeneralSettingsContainer.h
    LossModel/LossMethod.h
    LossModel/PelicunShared.h
    PerformanceMethod/PerformanceMethodSelection.h
    PerformanceMethod/REDiWidget.h
)

# ---- Define Target and add ----
#set(TARGET_NAME PBE)

add_executable(${PROJECT_NAME}
    ${APP_ICON}
    ${RESOURCES}
    ${FORMS}
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE CURL_STATICLIB USE_SIMCENTER_PYTHON)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        Advapi32 crypt32 ws2_32 user32
    )
    # Icon for Windows
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    target_sources(${PROJECT_NAME} PRIVATE icons/NHERI-PBE-Icon.ico)
    
elseif(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _GRAPHICS_Qt3D)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework SystemConfiguration" "-framework Security")
    
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE NHERI-PBE-Icon.icns
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    )
    # Architecture handling (Equivalent to QMAKE_APPLE_DEVICE_ARCHITECTURES)
    #set(CMAKE_OSX_ARCHITECTURES "arm64")
    
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE curl)
endif()


# =============================================================================
# Source-in modules (replacing .pri includes)
# =============================================================================

# Common (also pulls in ZipUtils via include + simcenter_add_ziputils)
include(${CMAKE_CURRENT_LIST_DIR}/../SimCenterCommon/Common/Common.cmake)
simcenter_add_common(${PROJECT_NAME})

# RandomVariables
include(${CMAKE_CURRENT_LIST_DIR}/../SimCenterCommon/RandomVariables/RandomVariables.cmake)
simcenter_add_randomvariables(${PROJECT_NAME})

# InputSheetBM
include(${CMAKE_CURRENT_LIST_DIR}/../SimCenterCommon/InputSheetBM/InputSheet.cmake)
simcenter_add_inputsheet(${PROJECT_NAME})

# Workflow
include(${CMAKE_CURRENT_LIST_DIR}/../SimCenterCommon/Workflow/Workflow.cmake)
simcenter_add_workflow(${PROJECT_NAME})

# Earthquake Events
include(${CMAKE_CURRENT_LIST_DIR}/../EE-UQ/EarthquakeEvents.cmake)
simcenter_add_earthquake_events(${PROJECT_NAME})

# Shark
include(${CMAKE_CURRENT_LIST_DIR}/../QS3hark/QS3hark.cmake)
simcenter_add_shark(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Charts Qt6::Concurrent 
    Qt6::Network Qt6::PrintSupport Qt6::WebEngineWidgets 
    Qt6::WebEngineCore Qt6::3DCore Qt6::3DRender Qt6::3DExtras
    ZLIB::ZLIB
    CURL::libcurl
    QuaZip::QuaZip
)

# ---- Platform-specific ----
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE CURL_STATICLIB)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        Advapi32 crypt32 ws2_32 user32
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    
elseif(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _GRAPHICS_Qt3D)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework SystemConfiguration" "-framework Security")
    
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_ICON_FILE NHERI-PBE-Icon.icns
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    )
    # Architecture handling (Equivalent to QMAKE_APPLE_DEVICE_ARCHITECTURES)
    # set(CMAKE_OSX_ARCHITECTURES "arm64")
    
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE curl)
endif()
