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

#include <QQuickWindow>

class QEventLoop;

class QuickDialog : public QQuickWindow
{
    Q_OBJECT
public:
    explicit QuickDialog(QQuickWindow *parent = nullptr);

    int exec();

Q_SIGNALS:
    void accepted();
    void rejected();

private:
    QEventLoop *m_loop;
};

#endif // QUICKDIALOG_H
