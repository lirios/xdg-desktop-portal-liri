import qbs 1.0
import qbs.File
import qbs.FileInfo
import qbs.ModUtils
import qbs.TextFile

Product {
    name: "xdg-desktop-portal-liri-data"
    type: ["dbus.service"]

    Depends { name: "lirideployment" }

    Group {
        name: "Portal"
        files: ["liri.portal"]
        qbs.install: true
        qbs.installDir: lirideployment.dataDir + "/xdg-desktop-portal/portals"
    }

    Group {
        name: "Service Templates"
        files: ["*.service.in"]
        fileTags: ["serviceIn"]
    }

    Rule {
        inputs: ["serviceIn"]

        Artifact {
            filePath: input.fileName.replace(".in", "")
            fileTags: ["dbus.service"]
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "creating " + output.fileName;
            cmd.highlight = "filegen";
            cmd.vars = {
                LIBEXECDIR: FileInfo.joinPaths(product.moduleProperty("qbs", "installPrefix"),
                                               product.moduleProperty("lirideployment", "libexecDir")),
            };
            cmd.sourceCode = function() {
                File.copy(input.filePath, output.filePath);

                var file = new TextFile(output.filePath, TextFile.ReadWrite);
                var contents = file.readAll();
                for (var i in vars)
                    contents = contents.replace(new RegExp('@' + i + '@(?!\w)', 'g'), vars[i]);
                file.truncate();
                file.write(contents);
                file.close();
            };
            return [cmd];
        }
    }

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.dataDir + "/dbus-1/services"
        fileTagsFilter: "dbus.service"
    }
}
