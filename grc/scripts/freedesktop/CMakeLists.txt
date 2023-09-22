# Copyright 2011-2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################

find_program(HAVE_XDG_UTILS xdg-desktop-menu)

if(UNIX AND HAVE_XDG_UTILS AND ENABLE_POSTINSTALL)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/grc_setup_freedesktop.in
        ${CMAKE_CURRENT_BINARY_DIR}/grc_setup_freedesktop
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    @ONLY)
   install (
        CODE "execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/grc_setup_freedesktop install)"
    )
endif(UNIX AND HAVE_XDG_UTILS AND ENABLE_POSTINSTALL)

if(UNIX AND ENABLE_TESTING)
    find_program(APPSTREAMCLI appstream-util)
    if(APPSTREAMCLI)
        GR_ADD_TEST(metainfo_test "${APPSTREAMCLI}" validate-relax "${CMAKE_CURRENT_SOURCE_DIR}/org.gnuradio.grc.metainfo.xml")
    endif(APPSTREAMCLI)
endif(UNIX AND ENABLE_TESTING)   
