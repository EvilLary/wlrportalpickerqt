cmake_minimum_required(VERSION 3.21)
project(wlrportalpicker VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

string(TOUPPER "${CMAKE_BUILD_TYPE}" UPPER_BUILD_TYPE)
if(UPPER_BUILD_TYPE STREQUAL "DEBUG")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(QT_QML_GENERATE_QMLLS_INI ON)
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Gui Qml Quick WaylandClient)
qt_standard_project_setup(REQUIRES 6.5)
qt_policy(SET QTP0004 NEW)

find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND_CLIENT REQUIRED IMPORTED_TARGET wayland-client)
pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols>=1.32)
pkg_get_variable(WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)

qt_add_executable(wlrportalpicker
    src/main.cpp
    src/toplevelmodel.cpp
    src/backend.cpp
)

qt_add_qml_module(wlrportalpicker
    URI WlrPortalPicker
    VERSION 1.0
    QML_FILES
        src/qml/Main.qml
)

qt_generate_wayland_protocol_client_sources(wlrportalpicker
    PRIVATE_CODE
    FILES
        ${WAYLAND_PROTOCOLS_DIR}/staging/ext-foreign-toplevel-list/ext-foreign-toplevel-list-v1.xml
)

target_include_directories(wlrportalpicker PRIVATE src)

target_link_libraries(wlrportalpicker PRIVATE
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::WaylandClient
    PkgConfig::WAYLAND_CLIENT
)

#include(GNUInstallDirs)
install(TARGETS wlrportalpicker)
