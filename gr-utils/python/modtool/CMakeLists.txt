# Copyright 2011 Free Software Foundation, Inc.
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
    cmakefile_editor.py
    code_generator.py
    grc_xml_generator.py
    modtool_add.py
    modtool_base.py
    modtool_disable.py
    modtool_help.py
    modtool_info.py
    modtool_makexml.py
    modtool_newmod.py
    modtool_rm.py
    modtool_rename.py
    parser_cc_block.py
    scm.py
    templates.py
    util_functions.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/modtool
    COMPONENT "utils"
)

set(GR_PKG_MODTOOL_DATA_DIR ${GR_PKG_DATA_DIR}/modtool)
install(DIRECTORY gr-newmod
    DESTINATION ${GR_PKG_MODTOOL_DATA_DIR}
    COMPONENT "utils"
)


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
    COMPONENT "utils"
)

