<!--
SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>

SPDX-License-Identifier: CC0-1.0
-->

xdg-desktop-portal-liri
=======================

[![License](https://img.shields.io/badge/license-GPLv3.0-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.html)
[![GitHub release](https://img.shields.io/github/release/lirios/xdg-desktop-portal-liri.svg)](https://github.com/lirios/xdg-desktop-portal-liri)
[![GitHub issues](https://img.shields.io/github/issues/lirios/xdg-desktop-portal-liri.svg)](https://github.com/lirios/xdg-desktop-portal-liri/issues)
[![CI](https://github.com/lirios/xdg-desktop-portal-liri/workflows/CI/badge.svg?branch=develop)](https://github.com/lirios/xdg-desktop-portal-liri/actions?query=workflow%3ACI)
[![REUSE status](https://api.reuse.software/badge/github.com/lirios/xdg-desktop-portal-liri)](https://api.reuse.software/info/github.com/lirios/xdg-desktop-portal-liri)

A backend implementation of [xdg-desktop-portal](https://github.com/flatpak/xdg-desktop-portal)
that is using Qt and Liri.

## Dependencies

Make sure you have Qt >= 5.10.0 with the following modules:

 * [qtbase](http://code.qt.io/cgit/qt/qtbase.git)
 * [qtdeclarative](http://code.qt.io/cgit/qt/qtdeclarative.git)
 * [qtquickcontrols2](http://code.qt.io/cgit/qt/qtquickcontrols2.git)

And the following modules:

 * [cmake](https://gitlab.kitware.com/cmake/cmake) >= 3.10.0
 * [cmake-shared](https://github.com/lirios/cmake-shared.git) >= 1.0.0
 * [qtaccountsservice](https://github.com/lirios/qtaccountsservice) >= 1.2.0
 * [qtgsettings](https://github.com/lirios/qtgsettings) >= 1.1.0
 * [libliri](https://github.com/lirios/libliri.git)
 * [aurora-client](https://github.com/lirios/aurora-client)

Optional dependencies:

 * [pipewire](https://gitlab.freedesktop.org/pipewire/pipewire/) >= 0.3
 * [libdrm](https://wiki.freedesktop.org/dri/) (required when pipewire is found)

Runtime dependencies:

 * [flatpak](https://github.com/flatpak/flatpak)
 * [xdg-desktop-portal](https://github.com/flatpak/xdg-desktop-portal)

## Installation

```sh
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/path/to/prefix ..
make
make install # use sudo if necessary
```

Replace `/path/to/prefix` to your installation prefix.
Default is `/usr/local`.

You can also append the following options to the `cmake` command:

 * `-DLIRI_ENABLE_SYSTEMD:BOOL=OFF`: Disable systemd support.
 * `-DINSTALL_SYSTEMDUSERUNITDIR=/path/to/systemd/user`: Path to install systemd user units (default: `/usr/local/lib/systemd/user`).

## Licensing

Licensed under the terms of the GNU General Public License version 3.0 or,
at your option, any later version.
