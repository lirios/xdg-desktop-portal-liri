cmake_minimum_required(VERSION 3.10.0)

project("XdgDesktopPortalLiri"
    VERSION "0.0.0"
    DESCRIPTION "xdg-desktop-portal backend for Liri"
    LANGUAGES CXX C
)

## Shared macros and functions:
if(NOT LIRI_LOCAL_ECM)
    find_package(LiriCMakeShared "1.0.0" REQUIRED NO_MODULE)
    list(APPEND CMAKE_MODULE_PATH "${LCS_MODULE_PATH}")
endif()

## Set minimum versions required:
set(QT_MIN_VERSION "5.10.0")

## Liri specific setup common for all modules:
include(LiriSetup)

## Find Qt 5:
find_package(Qt5 "${QT_MIN_VERSION}"
    CONFIG REQUIRED
    COMPONENTS
        Core
        DBus
        PrintSupport
        Qml
        Quick
        QuickControls2
)

## Add subdirectories:
add_subdirectory(data)
add_subdirectory(src)
