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

#include <QEventLoop>

#include "quickdialog.h"

QuickDialog::QuickDialog(QQuickWindow *parent)
    : QQuickWindow(parent)
    , m_loop(new QEventLoop(this))
{
    connect(this, &QuickDialog::accepted, this, [this] {
        m_loop->exit(0);
    });
    connect(this, &QuickDialog::rejected, this, [this] {
        m_loop->exit(1);
    });
}

int QuickDialog::exec()
{
    return m_loop->exec();
}