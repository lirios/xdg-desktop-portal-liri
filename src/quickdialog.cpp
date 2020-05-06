// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QEventLoop>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWindow>

#include "desktopportal.h"
#include "quickdialog.h"
#include "logging_p.h"

QuickDialog::QuickDialog(const QUrl &url, QObject *parent)
    : QObject(parent)
    , m_context(new QQmlContext(DesktopPortal::instance()->engine(), this))
    , m_component(new QQmlComponent(DesktopPortal::instance()->engine(), url, this))
    , m_loop(new QEventLoop(this))
{
    if (m_component->isReady()) {
        handleReady();
    } else if (m_component->isError()) {
        qCWarning(lcPortal) << "Unable to load QML component:" << m_component->errorString();
    } else {
        m_statusConnection =
                connect(m_component, &QQmlComponent::statusChanged,
                        this, &QuickDialog::handleStatusChanged);
    }
}

QString QuickDialog::title() const
{
    if (m_rootObject)
        return m_rootObject->title();
    return QString();
}

void QuickDialog::setTitle(const QString &title)
{
    if (m_rootObject)
        m_rootObject->setTitle(title);
}

bool QuickDialog::isModal() const
{
    return m_modal;
}

void QuickDialog::setModal(bool value)
{
    if (m_modal == value)
        return;

    m_modal = value;

    if (m_rootObject)
        m_rootObject->setProperty("modal", m_modal);

    Q_EMIT modalChanged();
}

QQmlContext *QuickDialog::rootContext() const
{
    return m_context;
}

QObject *QuickDialog::rootObject() const
{
    return m_rootObject;
}

bool QuickDialog::exec()
{
    if (m_rootObject)
        m_rootObject->show();
    return m_loop->exec() == 0;
}

void QuickDialog::handleStatusChanged(QQmlComponent::Status status)
{
    if (status == QQmlComponent::Ready) {
        disconnect(m_statusConnection);
        handleReady();
    }
}

void QuickDialog::handleReady()
{
    m_rootObject = qobject_cast<QQuickWindow *>(m_component->create(m_context));
    connect(m_rootObject, &QQuickWindow::windowTitleChanged, this, &QuickDialog::titleChanged);
    connect(m_rootObject, SIGNAL(accepted()), this, SLOT(handleAccepted()));
    connect(m_rootObject, SIGNAL(rejected()), this, SLOT(handleRejected()));
}

void QuickDialog::handleAccepted()
{
    m_loop->exit(0);
    if (m_rootObject)
        m_rootObject->close();
}

void QuickDialog::handleRejected()
{
    m_loop->exit(1);
    if (m_rootObject)
        m_rootObject->close();
}
