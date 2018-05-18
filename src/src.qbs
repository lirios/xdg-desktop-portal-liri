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
    ]

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.libexecDir
        fileTagsFilter: product.type
    }
}
