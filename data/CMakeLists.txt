install(FILES "liri.portal" DESTINATION "${INSTALL_DATADIR}/xdg-desktop-portal/portals")

if(IS_ABSOLUTE "${INSTALL_LIBEXECDIR}")
    set(ABSOLUTE_LIBEXECDIR "${INSTALL_LIBEXECDIR}")
else()
    set(ABSOLUTE_LIBEXECDIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBEXECDIR}")
endif()
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/org.freedesktop.impl.portal.desktop.liri.service.in"
    "${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.impl.portal.desktop.liri.service"
    @ONLY
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.impl.portal.desktop.liri.service"
        DESTINATION "${INSTALL_DATADIR}/dbus-1/services")

if(LIRI_ENABLE_SYSTEMD)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/xdg-desktop-portal-liri.service.in"
        "${CMAKE_CURRENT_BINARY_DIR}/xdg-desktop-portal-liri.service"
        @ONLY
    )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/xdg-desktop-portal-liri.service"
            DESTINATION "${INSTALL_SYSTEMDUSERUNITDIR}")
endif()
