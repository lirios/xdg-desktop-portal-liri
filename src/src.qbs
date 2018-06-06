import qbs 1.0

QtGuiApplication {
    name: targetName
    targetName: "xdg-desktop-portal-liri"

    Depends { name: "lirideployment" }
    Depends { name: "Qt"; submodules: ["core", "dbus", "printsupport"] }
    Depends { name: "LiriNotifications" }

    cpp.defines: [
        'VERSION="' + project.version + '"',
        "QT_NO_CAST_FROM_ASCII",
        "QT_NO_CAST_TO_ASCII"
    ]

    files: [
        "accessportal.cpp",
        "accessportal.h",
        "appchooserportal.cpp",
        "appchooserportal.h",
        "desktopportal.cpp",
        "desktopportal.h",
        "emailportal.cpp",
        "emailportal.h",
        "filechooserportal.cpp",
        "filechooserportal.h",
        "inhibitportal.cpp",
        "inhibitportal.h",
        "logging.cpp",
        "logging_p.h",
        "main.cpp",
        "notificationportal.cpp",
        "notificationportal.h",
        "printportal.cpp",
        "printportal.h",
        "screencastportal.cpp",
        "screencastportal.h",
        "screenshotportal.cpp",
        "screenshotportal.h",
    ]

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.libexecDir
        fileTagsFilter: product.type
    }
}
