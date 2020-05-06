// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-FileCopyrightText: 2016-2018 Red Hat, Inc
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDBusArgument>
#include <QDBusMetaType>
#include <QFileDialog>

#include "filechooserportal.h"
#include "logging_p.h"

// Read the specifications here:
// https://github.com/flatpak/xdg-desktop-portal/blob/master/data/org.freedesktop.impl.portal.FileChooser.xml

// Keep in sync with qflatpakfiledialog from flatpak-platform-plugin
Q_DECLARE_METATYPE(FileChooserPortal::Filter)
Q_DECLARE_METATYPE(FileChooserPortal::Filters)
Q_DECLARE_METATYPE(FileChooserPortal::FilterList)
Q_DECLARE_METATYPE(FileChooserPortal::FilterListList)

QDBusArgument &operator<<(QDBusArgument &arg, const FileChooserPortal::Filter &filter)
{
    arg.beginStructure();
    arg << filter.type << filter.filterString;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, FileChooserPortal::Filter &filter)
{
    uint type;
    QString filterString;
    arg.beginStructure();
    arg >> type >> filterString;
    filter.type = type;
    filter.filterString = filterString;
    arg.endStructure();

    return arg;
}

QDBusArgument &operator<<(QDBusArgument &arg, const FileChooserPortal::FilterList &filterList)
{
    arg.beginStructure();
    arg << filterList.userVisibleName << filterList.filters;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, FileChooserPortal::FilterList &filterList)
{
    QString userVisibleName;
    FileChooserPortal::Filters filters;
    arg.beginStructure();
    arg >> userVisibleName >> filters;
    filterList.userVisibleName = userVisibleName;
    filterList.filters = filters;
    arg.endStructure();

    return arg;
}

FileChooserPortal::FileChooserPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

quint32 FileChooserPortal::OpenFile(const QDBusObjectPath &handle,
                                    const QString &app_id,
                                    const QString &parent_window,
                                    const QString &title,
                                    const QVariantMap &options,
                                    QVariantMap &results)
{
    qCDebug(lcFileChooser) << "OpenFile called with parameters:";
    qCDebug(lcFileChooser) << "    handle: " << handle.path();
    qCDebug(lcFileChooser) << "    app_id: " << app_id;
    qCDebug(lcFileChooser) << "    parent_window: " << parent_window;
    qCDebug(lcFileChooser) << "    title: " << title;
    qCDebug(lcFileChooser) << "    options: " << options;

    bool modal = false;
    bool multiple = false;
    QString acceptLabel;
    QString currentFilter;
    QStringList nameFilters;
    QStringList mimeTypeFilters;

    if (options.contains(QStringLiteral("modal")))
        modal = options.value(QStringLiteral("modal")).toBool();
    if (options.contains(QStringLiteral("multiple")))
        multiple = options.value(QStringLiteral("multiple")).toBool();
    if (options.contains(QStringLiteral("accept_label")))
        acceptLabel = options.value(QStringLiteral("accept_label")).toString();
    if (options.contains(QStringLiteral("current_filter"))) {
        // current_filter sa(us)
        FilterList filterList = qdbus_cast<FilterList>(options.value(QStringLiteral("current_filter")));
        for (const Filter &filter : filterList.filters) {
            if (filter.type == 0) {
                currentFilter = filter.filterString;
                break;
            }
        }
    }
    if (options.contains(QStringLiteral("filters"))) {
        // filters a(sa(us))
        // A list of serialized file filters
        FilterListList filterListList = qdbus_cast<FilterListList>(options.value(QStringLiteral("filters")));
        for (const FilterList &filterList : filterListList) {
            QStringList filterStrings;
            for (const Filter &filter : filterList.filters) {
                if (filter.type == 0)
                    filterStrings.append(filter.filterString);
                else
                    mimeTypeFilters.append(filter.filterString);
            }

            if (!filterStrings.isEmpty()) {
                nameFilters.append(QStringLiteral("%1 (%2)")
                                   .arg(filterList.userVisibleName)
                                   .arg(filterStrings.join(QLatin1Char(' '))));
            }
        }
    }

    // The following options are not supported:
    // - handle_token
    // - choices

    auto *fileDialog = new QFileDialog();
    fileDialog->setWindowTitle(title);
    fileDialog->setModal(modal);
    fileDialog->setFileMode(multiple ? QFileDialog::ExistingFiles : QFileDialog::ExistingFile);

    if (!acceptLabel.isEmpty())
        fileDialog->setLabelText(QFileDialog::Accept, acceptLabel);
    if (!mimeTypeFilters.isEmpty())
        fileDialog->setMimeTypeFilters(mimeTypeFilters);
    if (!nameFilters.isEmpty()) {
        fileDialog->setNameFilters(nameFilters);
        if (!currentFilter.isEmpty())
            fileDialog->selectNameFilter(currentFilter);
    }

    if (fileDialog->exec() == QDialog::Accepted) {
        QStringList uris;
        for (const auto &fileName : fileDialog->selectedFiles()) {
            auto url = QUrl::fromLocalFile(fileName);
            uris << url.toDisplayString();
        }

        fileDialog->deleteLater();

        if (uris.isEmpty()) {
            qCDebug(lcFileChooser) << "Failed to open file: no file selected";
            return 2;
        }

        results.insert(QStringLiteral("uris"), uris);
        results.insert(QStringLiteral("writable"), true);

        return 0;
    }

    return 1;
}

quint32 FileChooserPortal::SaveFile(const QDBusObjectPath &handle,
                                    const QString &app_id,
                                    const QString &parent_window,
                                    const QString &title,
                                    const QVariantMap &options,
                                    QVariantMap &results)
{
    qCDebug(lcFileChooser) << "SaveFile called with parameters:";
    qCDebug(lcFileChooser) << "    handle: " << handle.path();
    qCDebug(lcFileChooser) << "    app_id: " << app_id;
    qCDebug(lcFileChooser) << "    parent_window: " << parent_window;
    qCDebug(lcFileChooser) << "    title: " << title;
    qCDebug(lcFileChooser) << "    options: " << options;

    bool modal = false;
    QString acceptLabel;
    QString currentName;
    QString currentFolder;
    QString currentFile;
    QString currentFilter;
    QStringList nameFilters;
    QStringList mimeTypeFilters;

    if (options.contains(QStringLiteral("modal")))
        modal = options.value(QStringLiteral("modal")).toBool();
    if (options.contains(QStringLiteral("accept_label")))
        acceptLabel = options.value(QStringLiteral("accept_label")).toString();
    if (options.contains(QStringLiteral("current_name")))
        currentName = options.value(QStringLiteral("current_name")).toString();
    if (options.contains(QStringLiteral("current_folder")))
        currentFolder = QString::fromUtf8(options.value(QStringLiteral("current_folder")).toByteArray());
    if (options.contains(QStringLiteral("current_file")))
        currentFile = QString::fromUtf8(options.value(QStringLiteral("current_file")).toByteArray());
    if (options.contains(QStringLiteral("current_filter"))) {
        // current_filter sa(us)
        FilterList filterList = qdbus_cast<FilterList>(options.value(QStringLiteral("current_filter")));
        for (const Filter &filter : filterList.filters) {
            if (filter.type == 0) {
                currentFilter = filter.filterString;
                break;
            }
        }
    }
    if (options.contains(QStringLiteral("filters"))) {
        // filters a(sa(us))
        // A list of serialized file filters
        FilterListList filterListList = qdbus_cast<FilterListList>(options.value(QStringLiteral("filters")));
        for (const FilterList &filterList : filterListList) {
            QStringList filterStrings;
            for (const Filter &filter : filterList.filters) {
                if (filter.type == 0)
                    filterStrings.append(filter.filterString);
                else
                    mimeTypeFilters.append(filter.filterString);
            }

            if (!filterStrings.isEmpty()) {
                nameFilters.append(QStringLiteral("%1 (%2)")
                                   .arg(filterList.userVisibleName)
                                   .arg(filterStrings.join(QLatin1Char(' '))));
            }
        }
    }

    // The following options are not supported:
    // - handle_token
    // - choices

    auto *fileDialog = new QFileDialog();
    fileDialog->setWindowTitle(title);
    fileDialog->setModal(modal);
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);

    if (!acceptLabel.isEmpty())
        fileDialog->setLabelText(QFileDialog::Accept, acceptLabel);
    if (!currentFolder.isEmpty())
        fileDialog->setDirectoryUrl(QUrl::fromLocalFile(currentFolder));
    if (!currentFile.isEmpty())
        fileDialog->selectFile(currentFile);
    if (!currentName.isEmpty())
        fileDialog->selectFile(currentName);
    if (!mimeTypeFilters.isEmpty())
        fileDialog->setMimeTypeFilters(mimeTypeFilters);
    if (!nameFilters.isEmpty()) {
        fileDialog->setNameFilters(nameFilters);
        if (!currentFilter.isEmpty())
            fileDialog->selectNameFilter(currentFilter);
    }

    if (fileDialog->exec() == QDialog::Accepted) {
        QStringList uris;
        for (const auto &fileName : fileDialog->selectedFiles()) {
            auto url = QUrl::fromLocalFile(fileName);
            uris << url.toDisplayString();
        }
        results.insert(QStringLiteral("uris"), uris);

        fileDialog->deleteLater();

        return 0;
    }

    return 1;
}

quint32 FileChooserPortal::SaveFiles(const QDBusObjectPath &handle,
                                     const QString &app_id,
                                     const QString &parent_window,
                                     const QString &title,
                                     const QVariantMap &options,
                                     QVariantMap &results)
{
    qCDebug(lcFileChooser) << "SaveFiles called with parameters:";
    qCDebug(lcFileChooser) << "    handle: " << handle.path();
    qCDebug(lcFileChooser) << "    app_id: " << app_id;
    qCDebug(lcFileChooser) << "    parent_window: " << parent_window;
    qCDebug(lcFileChooser) << "    title: " << title;
    qCDebug(lcFileChooser) << "    options: " << options;

    bool modal = false;
    QString acceptLabel;
    QString currentFolder;
    QStringList files;

    if (options.contains(QStringLiteral("modal")))
        modal = options.value(QStringLiteral("modal")).toBool();
    if (options.contains(QStringLiteral("accept_label")))
        acceptLabel = options.value(QStringLiteral("accept_label")).toString();
    if (options.contains(QStringLiteral("current_folder")))
        currentFolder = QString::fromUtf8(options.value(QStringLiteral("current_folder")).toByteArray());
    if (options.contains(QStringLiteral("files"))) {
        auto list = options.value(QStringLiteral("files")).toList();
        for (const auto &item : qAsConst(list))
            files.append(QString::fromUtf8(item.toByteArray()));
    }

    // The following options are not supported:
    // - handle_token
    // - choices

    auto *fileDialog = new QFileDialog();
    fileDialog->setWindowTitle(title);
    fileDialog->setModal(modal);
    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog->setOptions(QFileDialog::ShowDirsOnly);

    if (!acceptLabel.isEmpty())
        fileDialog->setLabelText(QFileDialog::Accept, acceptLabel);
    if (!currentFolder.isEmpty())
        fileDialog->setDirectoryUrl(QUrl::fromLocalFile(currentFolder));

    if (fileDialog->exec() == QDialog::Accepted) {
        QDir baseDir = fileDialog->directory();
        QStringList uris;
        for (const auto &fileName : qAsConst(files)) {
            // If that file already exist, we append a number
            int serial = 0;
            QString newFileName = fileName;
            while (baseDir.exists(newFileName))
                newFileName = QStringLiteral("%1 (%2)").arg(fileName, ++serial);

            auto url = QUrl::fromLocalFile(newFileName);
            uris << url.toDisplayString();
        }
        results.insert(QStringLiteral("uris"), uris);

        fileDialog->deleteLater();

        return 0;
    }

    return 1;
}
