import qbs 1.0

Project {
    name: "XdgDesktopPortalLiri"

    readonly property string version: "0.0.0"

    property bool useStaticAnalyzer: false

    condition: qbs.targetOS.contains("linux") && !qbs.targetOS.contains("android")

    minimumQbsVersion: "1.8.0"

    references: [
        "data/data.qbs",
        "src/src.qbs",
    ]
}
