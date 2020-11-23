# Copyright 2011-2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
set(grc_freedesktop_path ${GR_PKG_DATA_DIR}/grc/freedesktop)

install(FILES
    grc-icon-256.png
    grc-icon-128.png
    grc-icon-64.png
    grc-icon-48.png
    grc-icon-32.png
    grc-icon-24.png
    grc-icon-16.png
    gnuradio-grc.xml
    gnuradio-grc.desktop
    DESTINATION ${grc_freedesktop_path}
)

# Install desktop
install(FILES gnuradio-grc.desktop DESTINATION share/applications)

# Install mime
install(FILES gnuradio-grc.xml DESTINATION share/mime/packages)

# Install appstream / metainfo file
install(FILES org.gnuradio.grc.metainfo.xml DESTINATION share/metainfo)

# Install icons
install(FILES grc-icon-256.png DESTINATION share/icons/hicolor/256x256/apps RENAME gnuradio-grc.png)
install(FILES grc-icon-128.png DESTINATION share/icons/hicolor/128x128/apps RENAME gnuradio-grc.png)
install(FILES grc-icon-64.png DESTINATION share/icons/hicolor/64x64/apps RENAME gnuradio-grc.png)
install(FILES grc-icon-48.png DESTINATION share/icons/hicolor/48x48/apps RENAME gnuradio-grc.png)
install(FILES grc-icon-32.png DESTINATION share/icons/hicolor/32x32/apps RENAME gnuradio-grc.png)
install(FILES grc-icon-24.png DESTINATION share/icons/hicolor/24x24/apps RENAME gnuradio-grc.png)
install(FILES grc-icon-16.png DESTINATION share/icons/hicolor/16x16/apps RENAME gnuradio-grc.png)

find_program(HAVE_XDG_UTILS xdg-desktop-menu)

if(UNIX AND HAVE_XDG_UTILS)
    set(SRCDIR ${CMAKE_INSTALL_PREFIX}/${grc_freedesktop_path})
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/grc_setup_freedesktop.in
        ${CMAKE_CURRENT_BINARY_DIR}/grc_setup_freedesktop
    @ONLY)
    install(
        PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/grc_setup_freedesktop
        DESTINATION ${GR_PKG_LIBEXEC_DIR}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    )
    if(ENABLE_POSTINSTALL)
        install (
            CODE "execute_process(COMMAND ${CMAKE_INSTALL_PREFIX}/${GR_PKG_LIBEXEC_DIR}/grc_setup_freedesktop install)"
    )
    endif(ENABLE_POSTINSTALL)
endif(UNIX AND HAVE_XDG_UTILS)

if(UNIX AND ENABLE_TESTING)
    find_program(APPSTREAMCLI appstreamcli)
    if(APPSTREAMCLI)
        GR_ADD_TEST(metainfo_test "${APPSTREAMCLI}" validate "${CMAKE_CURRENT_SOURCE_DIR}/org.gnuradio.grc.metainfo.xml")
    endif(APPSTREAMCLI)
endif(UNIX AND ENABLE_TESTING)   
