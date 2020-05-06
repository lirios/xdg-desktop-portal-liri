// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef QUICKDIALOG_H
#define QUICKDIALOG_H

#include <QObject>
#include <QQmlComponent>

class QEventLoop;
class QQmlContext;
class QQuickWindow;

class QuickDialog : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool modal READ isModal WRITE setModal NOTIFY modalChanged)
public:
    explicit QuickDialog(const QUrl &url, QObject *parent = nullptr);

    QString title() const;
    void setTitle(const QString &title);

    bool isModal() const;
    void setModal(bool value);

    QQmlContext *rootContext() const;
    QObject *rootObject() const;

    bool exec();

Q_SIGNALS:
    void titleChanged();
    void modalChanged();

private:
    QQmlContext *m_context = nullptr;
    QQmlComponent *m_component = nullptr;
    QQuickWindow *m_rootObject = nullptr;
    QMetaObject::Connection m_statusConnection;
    QString m_title;
    bool m_modal = false;
    QEventLoop *m_loop;

private Q_SLOTS:
    void handleStatusChanged(QQmlComponent::Status status);
    void handleReady();
    void handleAccepted();
    void handleRejected();
};

#endif // QUICKDIALOG_H
