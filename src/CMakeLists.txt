# SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
#
# SPDX-License-Identifier: MIT

if(NOT TARGET Liri::Qt5AccountsService)
    find_package(Qt5AccountsService REQUIRED)
endif()
if(NOT TARGET Liri::Qt5GSettings)
    find_package(Qt5GSettings REQUIRED)
endif()
if(NOT TARGET Liri::Notifications)
    find_package(Liri1Notifications REQUIRED)
endif()
if(NOT TARGET Liri::WaylandClient)
    find_package(Liri1WaylandClient REQUIRED)
endif()
if(NOT TARGET Liri::Xdg)
    find_package(Liri1Xdg REQUIRED)
endif()

find_package(PipeWire)

if(TARGET PkgConfig::PipeWire)
    find_package(Libdrm REQUIRED)
endif()

set(screencast_SOURCES
    screencaststream.cpp
    screencaststream.h
)

liri_add_executable(XdgDesktopPortalLiri
    OUTPUT_NAME
        "xdg-desktop-portal-liri"
    SOURCES
        accessportal.cpp
        accessportal.h
        accountportal.cpp
        accountportal.h
        appchooserportal.cpp
        appchooserportal.h
        backgroundportal.cpp
        backgroundportal.h
        desktopportal.cpp
        desktopportal.h
        emailportal.cpp
        emailportal.h
        filechooserportal.cpp
        filechooserportal.h
        implementation/appsmodel.cpp
        implementation/appsmodel.h
        inhibitportal.cpp
        inhibitportal.h
        lockdownportal.cpp
        lockdownportal.h
        logging.cpp
        logging_p.h
        main.cpp
        notificationportal.cpp
        notificationportal.h
        printportal.cpp
        printportal.h
        quickdialog.cpp
        quickdialog.h
        request.cpp
        request.h
        session.cpp
        session.h
        screencastportal.cpp
        screencastportal.h
        screenshotclient.cpp
        screenshotclient.h
        screenshotimageprovider.cpp
        screenshotimageprovider.h
        screenshotportal.cpp
        screenshotportal.h
        settingsportal.cpp
        settingsportal.h
        wallpaperportal.cpp
        wallpaperportal.h
        waylandintegration.cpp
        waylandintegration.h
    RESOURCES
        "resources.qrc"
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
        VERSION="${PROJECT_VERSION}"
    INSTALL_DIRECTORY
        "${INSTALL_LIBEXECDIR}"
    LIBRARIES
        Qt5::Core
        Qt5::DBus
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Qml
        Qt5::Quick
        Qt5::QuickControls2
        Liri::Qt5AccountsService
        Liri::Qt5GSettings
        Liri::Notifications
        Liri::WaylandClient
        Liri::Xdg
)

if(TARGET PkgConfig::PipeWire)
    liri_extend_target("XdgDesktopPortalLiri"
        DEFINES SCREENCAST_ENABLED
        SOURCES "${screencast_SOURCES}"
        LIBRARIES "PkgConfig::PipeWire;PkgConfig::Libdrm")
endif()