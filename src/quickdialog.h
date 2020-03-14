/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef QUICKDIALOG_H
#define QUICKDIALOG_H

#include <QQuickView>

class QEventLoop;

class QuickDialog : public QQuickView
{
    Q_OBJECT
    Q_PROPERTY(bool modal READ isModal WRITE setModal NOTIFY modalChanged)
public:
    explicit QuickDialog(QWindow *parent = nullptr);

    bool isModal() const;
    void setModal(bool value);

    bool exec();

Q_SIGNALS:
    void modalChanged();

private:
    bool m_modal = false;
    QEventLoop *m_loop;

private Q_SLOTS:
    void handleAccepted();
    void handleRejected();
};

#endif // QUICKDIALOG_H
