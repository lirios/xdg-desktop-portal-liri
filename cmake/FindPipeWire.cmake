find_package(PkgConfig QUIET)
pkg_check_modules(PipeWire libpipewire-0.3 glib-2.0 IMPORTED_TARGET)
