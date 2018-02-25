# Copyright 2011-2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

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

# Install icons
install(FILES grc-icon-256.png DESTINATION share/icons/gnome/256x256/apps RENAME gnuradio-grc.png)
install(FILES grc-icon-128.png DESTINATION share/icons/gnome/128x128/apps RENAME gnuradio-grc.png)
install(FILES grc-icon-64.png DESTINATION share/icons/gnome/64x64/apps RENAME gnuradio-grc.png)
install(FILES grc-icon-48.png DESTINATION share/icons/gnome/48x48/apps RENAME gnuradio-grc.png)
install(FILES grc-icon-32.png DESTINATION share/icons/gnome/32x32/apps RENAME gnuradio-grc.png)
install(FILES grc-icon-24.png DESTINATION share/icons/gnome/24x24/apps RENAME gnuradio-grc.png)
install(FILES grc-icon-16.png DESTINATION share/icons/gnome/16x16/apps RENAME gnuradio-grc.png)
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
    )
endif(UNIX AND HAVE_XDG_UTILS)
