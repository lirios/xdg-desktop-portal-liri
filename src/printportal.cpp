// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-FileCopyrightText: 2016-2017 Jan Grulich <jgrulich@redhat.com>
// SPDX-FileCopyrightText: 2007,2010 John Layt <john@layt.net>
// SPDX-FileCopyrightText: 2007 Alex Merry <huntedhacker@tiscali.co.uk>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDateTime>
#include <QFile>
#include <QProcess>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintEngine>
#include <QSizeF>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QUrl>

#include "printportal.h"
#include "logging_p.h"

// Read the specifications here:
// https://github.com/flatpak/xdg-desktop-portal/blob/master/data/org.freedesktop.impl.portal.Print.xml
// https://github.com/flatpak/xdg-desktop-portal/blob/master/data/org.freedesktop.portal.Print.xml


// Standard sizes data
struct StandardPageSize {
    QPageSize::PageSizeId id;
    const char *mediaOption;  // PPD standard mediaOption ID
};

// Standard page sizes taken from the Postscript PPD Standard v4.3
// See https://www-cdf.fnal.gov/offline/PostScript/5003.PPD_Spec_v4.3.pdf
// Excludes all Transverse and Rotated sizes
// NB!This table needs to be in sync with QPageSize::PageSizeId
const static StandardPageSize qt_pageSizes[] = {
    // Existing Qt sizes including ISO, US, ANSI and other standards
    {QPageSize::A4, "A4"},
    {QPageSize::B5, "ISOB5"},
    {QPageSize::Letter, "Letter"},
    {QPageSize::Legal, "Legal"},
    {QPageSize::Executive, "Executive.7.5x10in"}, // Qt size differs from Postscript / Windows
    {QPageSize::A0, "A0"},
    {QPageSize::A1, "A1"},
    {QPageSize::A2, "A2"},
    {QPageSize::A3, "A3"},
    {QPageSize::A5, "A5"},
    {QPageSize::A6, "A6"},
    {QPageSize::A7, "A7"},
    {QPageSize::A8, "A8"},
    {QPageSize::A9, "A9"},
    {QPageSize::B0, "ISOB0"},
    {QPageSize::B1, "ISOB1"},
    {QPageSize::B10, "ISOB10"},
    {QPageSize::B2, "ISOB2"},
    {QPageSize::B3, "ISOB3"},
    {QPageSize::B4, "ISOB4"},
    {QPageSize::B6, "ISOB6"},
    {QPageSize::B7, "ISOB7"},
    {QPageSize::B8, "ISOB8"},
    {QPageSize::B9, "ISOB9"},
    {QPageSize::C5E, "EnvC5"},
    {QPageSize::Comm10E, "Env10"},
    {QPageSize::DLE, "EnvDL"},
    {QPageSize::Folio, "Folio"},
    {QPageSize::Ledger, "Ledger"},
    {QPageSize::Tabloid, "Tabloid"},
    {QPageSize::Custom, "Custom"}, // Special case to keep in sync with QPageSize::PageSizeId

    // ISO Standard Sizes
    {QPageSize::A10, "A10"},
    {QPageSize::A3Extra, "A3Extra"},
    {QPageSize::A4Extra, "A4Extra"},
    {QPageSize::A4Plus, "A4Plus"},
    {QPageSize::A4Small, "A4Small"},
    {QPageSize::A5Extra, "A5Extra"},
    {QPageSize::B5Extra, "ISOB5Extra"},

    // JIS Standard Sizes
    {QPageSize::JisB0, "B0"},
    {QPageSize::JisB1, "B1"},
    {QPageSize::JisB2, "B2"},
    {QPageSize::JisB3, "B3"},
    {QPageSize::JisB4, "B4"},
    {QPageSize::JisB5, "B5"},
    {QPageSize::JisB6, "B6"},
    {QPageSize::JisB7, "B7"},
    {QPageSize::JisB8, "B8"},
    {QPageSize::JisB9, "B9"},
    {QPageSize::JisB10, "B10"},

    // ANSI / US Standard sizes
    {QPageSize::AnsiC, "AnsiC"},
    {QPageSize::AnsiD, "AnsiD"},
    {QPageSize::AnsiE, "AnsiE"},
    {QPageSize::LegalExtra, "LegalExtra"},
    {QPageSize::LetterExtra, "LetterExtra"},
    {QPageSize::LetterPlus, "LetterPlus"},
    {QPageSize::LetterSmall, "LetterSmall"},
    {QPageSize::TabloidExtra, "TabloidExtra"},

    // Architectural sizes
    {QPageSize::ArchA, "ARCHA"},
    {QPageSize::ArchB, "ARCHB"},
    {QPageSize::ArchC, "ARCHC"},
    {QPageSize::ArchD, "ARCHD"},
    {QPageSize::ArchE, "ARCHE"},

    // Inch-based Sizes
    {QPageSize::Imperial7x9, "7x9"},
    {QPageSize::Imperial8x10, "8x10"},
    {QPageSize::Imperial9x11, "9x11"},
    {QPageSize::Imperial9x12, "9x12"},
    {QPageSize::Imperial10x11, "10x11"},
    {QPageSize::Imperial10x13, "10x13"},
    {QPageSize::Imperial10x14, "10x14"},
    {QPageSize::Imperial12x11, "12x11"},
    {QPageSize::Imperial15x11, "15x11"},

    // Other Page Sizes
    {QPageSize::ExecutiveStandard, "Executive"},   // Qt size differs from Postscript / Windows
    {QPageSize::Note, "Note"},
    {QPageSize::Quarto, "Quarto"},
    {QPageSize::Statement, "Statement"},
    {QPageSize::SuperA, "SuperA"},
    {QPageSize::SuperB, "SuperB"},
    {QPageSize::Postcard, "Postcard"},
    {QPageSize::DoublePostcard, "DoublePostcard"},
    {QPageSize::Prc16K, "PRC16K"},
    {QPageSize::Prc32K, "PRC32K"},
    {QPageSize::Prc32KBig, "PRC32KBig"},

    // Fan Fold Sizes
    {QPageSize::FanFoldUS, "FanFoldUS"},
    {QPageSize::FanFoldGerman, "FanFoldGerman"},
    {QPageSize::FanFoldGermanLegal, "FanFoldGermanLegal"},

    // ISO Envelopes
    {QPageSize::EnvelopeB4, "EnvISOB4"},
    {QPageSize::EnvelopeB5, "EnvISOB5"},
    {QPageSize::EnvelopeB6, "EnvISOB6"},
    {QPageSize::EnvelopeC0, "EnvC0"},
    {QPageSize::EnvelopeC1, "EnvC1"},
    {QPageSize::EnvelopeC2, "EnvC2"},
    {QPageSize::EnvelopeC3, "EnvC3"},
    {QPageSize::EnvelopeC4, "EnvC4"},
    {QPageSize::EnvelopeC6, "EnvC6"},
    {QPageSize::EnvelopeC65, "EnvC65"},
    {QPageSize::EnvelopeC7, "EnvC7"},

    // US Envelopes
    {QPageSize::Envelope9, "Env9"},
    {QPageSize::Envelope11, "Env11"},
    {QPageSize::Envelope12, "Env12"},
    {QPageSize::Envelope14, "Env14"},
    {QPageSize::EnvelopeMonarch, "EnvMonarch"},
    {QPageSize::EnvelopePersonal, "EnvPersonal"},

    // Other Envelopes
    {QPageSize::EnvelopeChou3, "EnvChou3"},
    {QPageSize::EnvelopeChou4, "EnvChou4"},
    {QPageSize::EnvelopeInvite, "EnvInvite"},
    {QPageSize::EnvelopeItalian, "EnvItalian"},
    {QPageSize::EnvelopeKaku2, "EnvKaku2"},
    {QPageSize::EnvelopeKaku3, "EnvKaku3"},
    {QPageSize::EnvelopePrc1, "EnvPRC1"},
    {QPageSize::EnvelopePrc2, "EnvPRC2"},
    {QPageSize::EnvelopePrc3, "EnvPRC3"},
    {QPageSize::EnvelopePrc4, "EnvPRC4"},
    {QPageSize::EnvelopePrc5, "EnvPRC5"},
    {QPageSize::EnvelopePrc6, "EnvPRC6"},
    {QPageSize::EnvelopePrc7, "EnvPRC7"},
    {QPageSize::EnvelopePrc8, "EnvPRC8"},
    {QPageSize::EnvelopePrc9, "EnvPRC9"},
    {QPageSize::EnvelopePrc10, "EnvPRC10"},
    {QPageSize::EnvelopeYou4, "EnvYou4"}
};

static QPageSize::PageSizeId pageSizeIdFromKey(const QString &ppdKey)
{
    if (ppdKey.isEmpty())
        return QPageSize::Custom;

    for (int i = 0; i <= int(QPageSize::LastPageSize); ++i) {
        if (QLatin1String(qt_pageSizes[i].mediaOption) == ppdKey)
            return qt_pageSizes[i].id;
    }

    return QPageSize::Custom;
}

static QString keyForPageSizeId(QPageSize::PageSizeId id)
{
    return QString::fromLatin1(qt_pageSizes[id].mediaOption);
}

static bool isCupsAvailable()
{
    // If CUPS is available, then QPrinter::numCopies() will always return 1
    // whereas if CUPS is not available, it will return the real number of copies.
    // This behavior is guaranteed never to change, so we can use it as a reliable
    // substitute of private APIs.
    QPrinter testPrinter;
    testPrinter.setNumCopies(2);
    return testPrinter.numCopies() == 1;
}

static QStringList destination(const QPrinter *printer, const QString &exe)
{
    if (exe == QLatin1String("lp"))
        return QStringList(QStringLiteral("-d")) << printer->printerName();
    if (exe == QLatin1String("lpr"))
        return QStringList(QStringLiteral("-P")) << printer->printerName();
    return QStringList();
}

static QStringList jobName(const QPrinter *printer, const QString &exe)
{
    if (!printer->docName().isEmpty()) {
        if (exe == QLatin1String("lp"))
            return QStringList(QStringLiteral("-t")) << printer->docName();

        if (exe.startsWith(QLatin1String("lpr"))) {
            const QString shortenedDocName = QString::fromUtf8(printer->docName().toUtf8().left(255));
            return QStringList(QStringLiteral("-J")) << shortenedDocName;
        }
    }

    return QStringList();
}

static QStringList optionDoubleSidedPrinting(const QPrinter *printer)
{
    switch (printer->duplex()) {
    case QPrinter::DuplexNone:
        return QStringList(QStringLiteral("-o")) << QStringLiteral("sides=one-sided");
    case QPrinter::DuplexAuto:
        if (printer->orientation() == QPrinter::Landscape)
            return QStringList(QStringLiteral("-o")) << QStringLiteral("sides=two-sided-short-edge");
        else
            return QStringList(QStringLiteral("-o")) << QStringLiteral("sides=two-sided-long-edge");
    case QPrinter::DuplexLongSide:
        return QStringList(QStringLiteral("-o")) << QStringLiteral("sides=two-sided-long-edge");
    case QPrinter::DuplexShortSide:
        return QStringList(QStringLiteral("-o")) << QStringLiteral("sides=two-sided-short-edge");
    default:
        return QStringList();
    }
}

static QStringList optionPageOrder(const QPrinter *printer)
{
    if (printer->pageOrder() == QPrinter::LastPageFirst)
        return QStringList(QStringLiteral("-o")) << QStringLiteral("outputorder=reverse");
    return QStringList(QStringLiteral("-o")) << QStringLiteral("outputorder=normal");
}

static QStringList optionCollateCopies(const QPrinter *printer)
{
    if (printer->collateCopies())
        return QStringList(QStringLiteral("-o")) << QStringLiteral("Collate=True");
    return QStringList(QStringLiteral("-o")) << QStringLiteral("Collate=False");
}

static QStringList optionCupsProperties(const QPrinter *printer)
{
    QStringList dialogOptions = printer->printEngine()->property(QPrintEngine::PrintEnginePropertyKey(0xfe00)).toStringList();
    QStringList args;

    for (int i = 0; i < dialogOptions.count(); i = i + 2) {
        // Ignore some CUPS properties as the PDF we are getting is already formatted with these
        if (dialogOptions[i] == QLatin1String("number-up") || dialogOptions[i] == QLatin1String("number-up-layout"))
            continue;

        if (dialogOptions[i + 1].isEmpty())
            args << QStringLiteral("-o") << dialogOptions[i];
        else
            args << QStringLiteral("-o") << dialogOptions[i] + QLatin1Char('=') + dialogOptions[i + 1];
    }

    return args;
}

static QStringList cupsOptions(const QPrinter *printer)
{
    QStringList args;

    if (!optionDoubleSidedPrinting(printer).isEmpty())
        args << optionDoubleSidedPrinting(printer);

    if (!optionPageOrder(printer).isEmpty())
        args << optionPageOrder(printer);

    if (!optionCollateCopies(printer).isEmpty())
        args << optionCollateCopies(printer);

    args << optionCupsProperties(printer);

    return args;
}

static QStringList printCommandArgs(const QPrinter *printer, const QString &exe, QPrinter::Orientation orientation)
{
    QStringList args;

    if (!destination(printer, exe).isEmpty())
        args << destination(printer, exe);

    if (!jobName(printer, exe).isEmpty())
        args << jobName(printer, exe);

    if (isCupsAvailable() && !cupsOptions(printer).isEmpty())
        args << cupsOptions(printer);

    if (exe == QLatin1String("lp"))
        args << QStringLiteral("--");

    return args;
}


PrintPortal::PrintPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

quint32 PrintPortal::Print(const QDBusObjectPath &handle,
                           const QString &app_id,
                           const QString &parent_window,
                           const QString &title,
                           const QDBusUnixFileDescriptor &fd,
                           const QVariantMap &options,
                           QVariantMap &results)
{
    Q_UNUSED(results)

    qCDebug(lcPrint) << "Print called with parameters:";
    qCDebug(lcPrint) << "    handle: " << handle.path();
    qCDebug(lcPrint) << "    app_id: " << app_id;
    qCDebug(lcPrint) << "    parent_window: " << parent_window;
    qCDebug(lcPrint) << "    title: " << title;
    qCDebug(lcPrint) << "    fd: " << fd.fileDescriptor();
    qCDebug(lcPrint) << "    options: " << options;

    const bool modal = options.value(QStringLiteral("modal"), true).toBool();

    // Open the file to print
    QFile file;
    if (file.open(fd.fileDescriptor(), QFile::ReadOnly)) {
        QPrinter *printer = nullptr;

        // Reuse the token if possible, otherwise use the last configured printer
        if (options.contains(QStringLiteral("token")))
            printer = m_printers.value(options.value(QStringLiteral("token")).toUInt());
        else if (m_printers.count() > 0)
            printer = m_printers.last();

        if (!printer) {
            qCWarning(lcPrint, "Unable to print: no printer can be used");
            return 1;
        }

        if (printer->outputFileName().isEmpty()) {
            // Print to a printer

            // Find the command to use for printing
            QString exe;
            if (!QStandardPaths::findExecutable(QStringLiteral("lpr-cups")).isEmpty()) {
                exe = QStringLiteral("lpr-cups");
            } else if (!QStandardPaths::findExecutable(QStringLiteral("lpr.cups")).isEmpty()) {
                exe = QStringLiteral("lpr.cups");
            } else if (!QStandardPaths::findExecutable(QStringLiteral("lpr")).isEmpty()) {
                exe = QStringLiteral("lpr");
            } else if (!QStandardPaths::findExecutable(QStringLiteral("lp")).isEmpty()) {
                exe = QStringLiteral("lp");
            } else {
                qCWarning(lcPrint, "Unable to print: couldn't find lpr command");
                return 1;
            }

            QTemporaryFile tempFile;
            if (tempFile.open()) {
                tempFile.write(file.readAll());
                tempFile.close();
            } else {
                qCWarning(lcPrint, "Unable to print: failed to create temporary file");
                return 1;
            }

            QStringList args = printCommandArgs(printer, exe, printer->orientation()) << tempFile.fileName();
            if (QProcess::execute(exe, args) <=0 ) {
                qCWarning(lcPrint, "Failed to print");
                file.close();
                return 1;
            }

            file.close();
            return 0;
        } else {
            // Print to a file

            if (QFile::exists(printer->outputFileName()))
                QFile::remove(printer->outputFileName());

            QFile outputFile(printer->outputFileName());
            if (outputFile.open(QFile::ReadWrite)) {
                outputFile.write(file.readAll());
                outputFile.close();
            } else {
                qCWarning(lcPrint, "Unable to print: couldn't open file for writing");
                file.close();
                return 1;
            }

            file.close();
            return 0;
        }
    } else {
        qCWarning(lcPrint, "Unable to print: couldn't read from file description");
    }

    return 1;
}

quint32 PrintPortal::PreparePrint(const QDBusObjectPath &handle,
                                  const QString &app_id,
                                  const QString &parent_window,
                                  const QString &title,
                                  const QVariantMap &settings,
                                  const QVariantMap &page_setup,
                                  const QVariantMap &options,
                                  QVariantMap &results)
{
    qCDebug(lcPrint) << "PreparePrint called with parameters:";
    qCDebug(lcPrint) << "    handle: " << handle.path();
    qCDebug(lcPrint) << "    app_id: " << app_id;
    qCDebug(lcPrint) << "    parent_window: " << parent_window;
    qCDebug(lcPrint) << "    title: " << title;
    qCDebug(lcPrint) << "    settings: " << settings;
    qCDebug(lcPrint) << "    page_setup: " << page_setup;
    qCDebug(lcPrint) << "    options: " << options;

    QPrinter *printer = new QPrinter();

    // Settings:

    const QString orientationSetting = settings.value(QStringLiteral("orientation")).toString();
    const QString paperFormat = settings.value(QStringLiteral("paper-format")).toString();
    const QString paperWidth = settings.value(QStringLiteral("paper-width")).toString();
    const QString paperHeight = settings.value(QStringLiteral("paper-height")).toString();
    const QString numCopies = settings.value(QStringLiteral("n-copies")).toString();
    const QString defaultSource = settings.value(QStringLiteral("default-source")).toString();
    const QString quality = settings.value(QStringLiteral("quality")).toString();
    const QString resolution = settings.value(QStringLiteral("resolution")).toString();
    const QString useColor = settings.value(QStringLiteral("use-color")).toString();
    const QString duplex = settings.value(QStringLiteral("duplex")).toString();
    const QString collate = settings.value(QStringLiteral("collate")).toString();
    const QString reverse = settings.value(QStringLiteral("reverse")).toString();
    const QString mediaType = settings.value(QStringLiteral("media-type")).toString();
    const QString dither = settings.value(QStringLiteral("dither")).toString();
    const QString scale = settings.value(QStringLiteral("scale")).toString();
    const QString printPages = settings.value(QStringLiteral("print-pages")).toString();
    const QString printRanges = settings.value(QStringLiteral("print-ranges")).toString();
    const QString pageSet = settings.value(QStringLiteral("page-set")).toString();
    const QString finishings = settings.value(QStringLiteral("finishings")).toString();
    const QString numberUp = settings.value(QStringLiteral("number-up")).toString();
    const QString numberUpLayout = settings.value(QStringLiteral("number-up-layout")).toString();
    const QString outputBin = settings.value(QStringLiteral("output-bin")).toString();
    const QString resolutionX = settings.value(QStringLiteral("resolution-x")).toString();
    const QString resolutionY = settings.value(QStringLiteral("resolution-y")).toString();
    const QString printerLpi = settings.value(QStringLiteral("printer-lpi")).toString();
    const QString outputBaseName = settings.value(QStringLiteral("output-basename")).toString();
    const QString outputFileFormat = settings.value(QStringLiteral("output-file-format")).toString();
    const QString outputUri = settings.value(QStringLiteral("output-uri")).toString();

    if (!orientationSetting.isEmpty()) {
        // Qt is missing reverse options and we treat them as non-reversed
        if (orientationSetting == QLatin1String("portrait") ||
                orientationSetting == QLatin1String("reverse_portrait"))
            printer->setOrientation(QPrinter::Portrait);
        else if (orientationSetting == QLatin1String("landscape") ||
                 orientationSetting == QLatin1String("reverse_landscape"))
            printer->setOrientation(QPrinter::Landscape);
    }

    if (!paperFormat.isEmpty())
        qCWarning(lcPrint, "Ignoring \"paper-format\" setting, because it's not supported");

    if (!paperWidth.isEmpty())
        qCWarning(lcPrint, "Ignoring \"paper-width\" setting, because it's not supported");

    if (!paperHeight.isEmpty())
        qCWarning(lcPrint, "Ignoring \"paper-height\" setting, because it's not supported");

    if (!numCopies.isEmpty())
        printer->setCopyCount(numCopies.toInt());

    if (!defaultSource.isEmpty())
        qCWarning(lcPrint, "Ignoring \"default-source\" setting, because it's not supported");

    if (!quality.isEmpty())
        qCWarning(lcPrint, "Ignoring \"quality\" setting, because it's not supported");

    if (!resolution.isEmpty())
        printer->setResolution(resolution.toInt());

    if (!useColor.isEmpty())
        printer->setColorMode(useColor == QLatin1String("yes") ? QPrinter::Color : QPrinter::GrayScale);

    if (!duplex.isEmpty()) {
        if (duplex == QLatin1String("simplex"))
            printer->setDuplex(QPrinter::DuplexNone);
        else if (duplex == QLatin1String("horizontal"))
            printer->setDuplex(QPrinter::DuplexShortSide);
        else if (duplex == QLatin1String("vertical"))
            printer->setDuplex(QPrinter::DuplexLongSide);
    }

    if (!collate.isEmpty())
        printer->setCollateCopies(collate == QLatin1String("yes"));

    if (!reverse.isEmpty())
        printer->setPageOrder(reverse == QLatin1String("yes") ? QPrinter::LastPageFirst : QPrinter::FirstPageFirst);

    if (!mediaType.isEmpty())
        qCWarning(lcPrint, "Ignoring \"media-type\" setting, because it's not supported");

    if (!dither.isEmpty())
        qCWarning(lcPrint, "Ignoring \"dither\" setting, because it's not supported");

    if (!scale.isEmpty())
        qCWarning(lcPrint, "Ignoring \"scale\" setting, because it's not supported");

    if (!printPages.isEmpty()) {
        if (printPages == QLatin1String("all"))
            printer->setPrintRange(QPrinter::AllPages);
        else if (printPages == QLatin1String("selection"))
            printer->setPrintRange(QPrinter::Selection);
        else if (printPages == QLatin1String("current"))
            printer->setPrintRange(QPrinter::CurrentPage);
        else if (printPages == QLatin1String("ranges"))
            printer->setPrintRange(QPrinter::PageRange);
    }

    if (!printRanges.isEmpty()) {
        // GNOME supports multiple ranges like 1-5,7,9,11-15 while Qt only
        // supports one range like 1-15.  We take the first and last value.
        const auto ranges = printRanges.split(QLatin1Char(','));
        if (ranges.count() > 0) {
            const auto firstRangeValues = ranges.first().split(QLatin1Char('-'));
            const auto lastRangeValues = ranges.last().split(QLatin1Char('-'));
            printer->setFromTo(firstRangeValues.first().toInt(), lastRangeValues.last().toInt());
        }
    }

    if (!pageSet.isEmpty())
        qCWarning(lcPrint, "Ignoring \"page-set\" setting, because it's not supported");

    if (!finishings.isEmpty())
        qCWarning(lcPrint, "Ignoring \"finishings\" setting, because it's not supported");

    if (!numberUp.isEmpty())
        qCWarning(lcPrint, "Ignoring \"number-up\" setting, because it's not supported");

    if (!numberUpLayout.isEmpty())
        qCWarning(lcPrint, "Ignoring \"number-up-layout\" setting, because it's not supported");

    if (!outputBin.isEmpty())
        qCWarning(lcPrint, "Ignoring \"output-bin\" setting, because it's not supported");

    if (!resolutionX.isEmpty())
        qCWarning(lcPrint, "Ignoring \"resolution-x\" setting, because it's not supported");

    if (!resolutionY.isEmpty())
        qCWarning(lcPrint, "Ignoring \"resolution-y\" setting, because it's not supported");

    if (!printerLpi.isEmpty())
        qCWarning(lcPrint, "Ignoring \"printer-lpi\" setting, because it's not supported");

    if (!outputUri.isEmpty()) {
        const QUrl uri(outputUri);
        const QString uriString = uri.toDisplayString();

        // Check whether URI is just a directory and we need to append the output-basename
        if (outputBaseName.isEmpty())
            printer->setOutputFileName(uriString);
        else if (!uriString.endsWith(outputBaseName) && uriString.endsWith(QLatin1Char('/')))
            printer->setOutputFileName(uriString + outputBaseName);

        // Qt only supports PDF
        if (!outputFileFormat.isEmpty() && outputFileFormat != QLatin1String("PDF"))
            qCWarning(lcPrint) << "Forcing PDF file format instead of" << outputFileFormat;
        printer->setOutputFormat(QPrinter::PdfFormat);
    }

    // Page setup:

    const QString ppdName = page_setup.value(QStringLiteral("PPDName")).toString();
    const QString name = page_setup.value(QStringLiteral("Name")).toString();
    const QString displayName = page_setup.value(QStringLiteral("DisplayName")).toString();
    const QString orientation = page_setup.value(QStringLiteral("Orientation")).toString();

    if (!ppdName.isEmpty())
        printer->setPageSize(QPageSize(pageSizeIdFromKey(ppdName)));

    if (!name.isEmpty() && ppdName.isEmpty())
        printer->setPageSize(QPageSize(pageSizeIdFromKey(name)));

    if (!displayName.isEmpty())
        printer->setPageSize(QPageSize(printer->pageLayout().pageSize().size(QPageSize::Millimeter),
                                       QPageSize::Millimeter, displayName));

    if (page_setup.contains(QStringLiteral("Width")) && page_setup.contains(QStringLiteral("Height"))) {
        QSizeF paperSize;
        paperSize.setWidth(page_setup.value(QStringLiteral("Width")).toReal());
        paperSize.setHeight(page_setup.value(QStringLiteral("Height")).toReal());
        printer->setPageSize(QPageSize(paperSize, QPageSize::Millimeter, displayName));
    }

    printer->pageLayout().setUnits(QPageLayout::Millimeter);
    if (page_setup.contains(QStringLiteral("MarginTop")))
        printer->pageLayout().setTopMargin(page_setup.value(QStringLiteral("MarginTop")).toReal());
    if (page_setup.contains(QStringLiteral("MarginBottom")))
        printer->pageLayout().setBottomMargin(page_setup.value(QStringLiteral("MarginBottom")).toReal());
    if (page_setup.contains(QStringLiteral("MarginLeft")))
        printer->pageLayout().setLeftMargin(page_setup.value(QStringLiteral("MarginLeft")).toReal());
    if (page_setup.contains(QStringLiteral("MarginRight")))
        printer->pageLayout().setRightMargin(page_setup.value(QStringLiteral("MarginRight")).toReal());

    if (!orientation.isEmpty()) {
        // Qt is missing reverse options and we treat them as non-reversed
        if (orientation == QLatin1String("portrait") ||
                orientation == QLatin1String("reverse_portrait"))
            printer->setPageOrientation(QPageLayout::Portrait);
        else if (orientation == QLatin1String("landscape") ||
                 orientation == QLatin1String("reverse_landscape"))
            printer->setPageOrientation(QPageLayout::Landscape);
    }

    // Print dialog
    QPrintDialog *dialog = new QPrintDialog(printer);
    if (options.contains(QStringLiteral("modal")))
        dialog->setModal(options.value(QStringLiteral("modal")).toBool());
    if (dialog->exec() == QDialog::Accepted) {
        QVariantMap resultingSettings;
        QVariantMap resultingPageSetup;

        // Settings:
        resultingSettings.insert(QStringLiteral("n-copies"), QString::number(printer->copyCount()));
        resultingSettings.insert(QStringLiteral("resolution"), QString::number(printer->resolution()));
        resultingSettings.insert(
                    QStringLiteral("use-color"),
                    printer->colorMode() == QPrinter::Color ? QLatin1String("yes") : QLatin1String("no"));
        switch (printer->duplex()) {
        case QPrinter::DuplexNone:
            resultingSettings.insert(QStringLiteral("duplex"),
                                     QLatin1String("simplex"));
            break;
        case QPrinter::DuplexShortSide:
            resultingSettings.insert(QStringLiteral("duplex"),
                                     QLatin1String("horizontal"));
            break;
        case QPrinter::DuplexLongSide:
            resultingSettings.insert(QStringLiteral("duplex"),
                                     QLatin1String("vertical"));
            break;
        default:
            break;
        }
        resultingSettings.insert(
                    QStringLiteral("collapse"),
                    printer->collateCopies() ? QLatin1String("yes") : QLatin1String("no"));
        resultingSettings.insert(
                    QStringLiteral("reverse"),
                    printer->pageOrder() == QPrinter::LastPageFirst
                    ? QLatin1String("yes") : QLatin1String("no"));
        if (printer->printRange() == QPrinter::AllPages) {
            resultingSettings.insert(QStringLiteral("print-pages"), QLatin1String("no"));
        } else if (printer->printRange() == QPrinter::Selection) {
            resultingSettings.insert(QStringLiteral("print-pages"), QLatin1String("selection"));
        } else if (printer->printRange() == QPrinter::CurrentPage) {
            resultingSettings.insert(QStringLiteral("print-pages"), QLatin1String("current"));
        } else if (printer->printRange() == QPrinter::PageRange) {
            const auto fromPageToIndex = printer->fromPage() ? printer->fromPage() - 1 : printer->fromPage();
            const auto toPageToIndex = printer->toPage() ? printer->toPage() - 1 : printer->toPage();

            resultingSettings.insert(QStringLiteral("print-pages"), QLatin1String("rages"));
            resultingSettings.insert(QStringLiteral("print-ranges"),
                                     QStringLiteral("%1-%2").arg(fromPageToIndex, toPageToIndex));
        }
        if (printer->outputFileName() == QPrinter::PdfFormat)
            resultingSettings.insert(QStringLiteral("output-file-format"), QLatin1String("pdf"));
        if (!printer->outputFileName().isEmpty())
            resultingSettings.insert(QStringLiteral("output-uri"),
                                     QUrl::fromLocalFile(printer->outputFileName()).toDisplayString());

        // Page setup:
        resultingPageSetup.insert(
                    QStringLiteral("PPDName"),
                    keyForPageSizeId(printer->pageLayout().pageSize().id()));
        resultingPageSetup.insert(
                    QStringLiteral("Name"),
                    keyForPageSizeId(printer->pageLayout().pageSize().id()));
        resultingPageSetup.insert(
                    QStringLiteral("DisplayName"),
                    keyForPageSizeId(printer->pageLayout().pageSize().id()));
        resultingPageSetup.insert(
                    QStringLiteral("Width"),
                    printer->pageLayout().pageSize().size(QPageSize::Millimeter).width());
        resultingPageSetup.insert(
                    QStringLiteral("Height"),
                    printer->pageLayout().pageSize().size(QPageSize::Millimeter).height());
        resultingPageSetup.insert(
                    QStringLiteral("MarginTop"),
                    printer->pageLayout().margins(QPageLayout::Millimeter).top());
        resultingPageSetup.insert(
                    QStringLiteral("MarginBottom"),
                    printer->pageLayout().margins(QPageLayout::Millimeter).bottom());
        resultingPageSetup.insert(
                    QStringLiteral("MarginLeft"),
                    printer->pageLayout().margins(QPageLayout::Millimeter).left());
        resultingPageSetup.insert(
                    QStringLiteral("MarginRight"),
                    printer->pageLayout().margins(QPageLayout::Millimeter).right());
        resultingPageSetup.insert(
                    QStringLiteral("Orientation"),
                    printer->pageLayout().orientation() == QPageLayout::Landscape
                    ? QLatin1String("landscape") : QLatin1String("portrait"));

        quint32 token = QDateTime::currentDateTime().toSecsSinceEpoch() >> 32;
        m_printers.insert(token, printer);

        results.insert(QStringLiteral("token"), token);
        results.insert(QStringLiteral("settings"), resultingSettings);
        results.insert(QStringLiteral("page-setup"), resultingPageSetup);

        dialog->deleteLater();
        return 0;
    } else {
        dialog->deleteLater();
        return 1;
    }

    return 0;
}
