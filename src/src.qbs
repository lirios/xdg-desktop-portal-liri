import qbs 1.0

QtGuiApplication {
    name: targetName
    targetName: "xdg-desktop-portal-liri"

    Depends { name: "lirideployment" }
    Depends { name: "Qt"; submodules: ["core", "dbus", "printsupport", "qml", "quick"] }
    Depends { name: "LiriNotifications" }
    //Depends { name: "LiriXdg" }

    cpp.defines: [
        'VERSION="' + project.version + '"',
        "QT_NO_CAST_FROM_ASCII",
        "QT_NO_CAST_TO_ASCII"
    ]

    Qt.core.resourcePrefix: "/"
    Qt.core.resourceSourceBase: sourceDirectory

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
        "implementation/appsmodel.cpp",
        "implementation/appsmodel.h",
        "inhibitportal.cpp",
        "inhibitportal.h",
        "logging.cpp",
        "logging_p.h",
        "main.cpp",
        "notificationportal.cpp",
        "notificationportal.h",
        "printportal.cpp",
        "printportal.h",
        "quickdialog.cpp",
        "quickdialog.h",
        "screencastportal.cpp",
        "screencastportal.h",
        "screenshotportal.cpp",
        "screenshotportal.h",
    ]

    Group {
        name: "Resource Data"
        files: [
            "qml/AppChooserDialog.qml",
        ]
        fileTags: ["qt.core.resource_data"]
    }

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.libexecDir
        fileTagsFilter: product.type
    }
}
