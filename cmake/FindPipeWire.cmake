# SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
#
# SPDX-License-Identifier: MIT

find_package(PkgConfig QUIET)
pkg_check_modules(PipeWire libpipewire-0.3 libspa-0.2 glib-2.0 IMPORTED_TARGET)