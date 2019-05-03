# Copyright 2018 Free Software Foundation, Inc.
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

include(GrPython)

GR_PYTHON_INSTALL(FILES
    __init__.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/modtool
)

set(GR_PKG_MODTOOL_DATA_DIR ${GR_PKG_DATA_DIR}/modtool/templates)


########################################################################
# Create and install the modtool conf file
########################################################################
file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${GR_PKG_MODTOOL_DATA_DIR}/gr-newmod newmoddir)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/modtool.conf.in
    ${CMAKE_CURRENT_BINARY_DIR}/modtool.conf
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/modtool.conf
    DESTINATION ${GR_PREFSDIR}
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(core)
add_subdirectory(cli)
add_subdirectory(tools)
add_subdirectory(templates)
add_subdirectory(tests)
