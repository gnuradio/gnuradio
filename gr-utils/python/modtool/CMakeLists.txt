# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

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
