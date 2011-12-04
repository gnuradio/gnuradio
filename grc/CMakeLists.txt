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

########################################################################
# Setup dependencies
########################################################################
include(GrPython)

GR_PYTHON_CHECK_MODULE("python >= 2.5"     sys          "sys.version.split()[0] >= '2.5'"           PYTHON_MIN_VER_FOUND)
GR_PYTHON_CHECK_MODULE("Cheetah >= 2.0.0"  Cheetah      "Cheetah.Version >= '2.0.0'"                CHEETAH_FOUND)
GR_PYTHON_CHECK_MODULE("lxml >= 1.3.6"     lxml.etree   "lxml.etree.LXML_VERSION >= (1, 3, 6, 0)"   LXML_FOUND)
GR_PYTHON_CHECK_MODULE("pygtk >= 2.10.0"   gtk          "gtk.pygtk_version >= (2, 10, 0)"           PYGTK_FOUND)
GR_PYTHON_CHECK_MODULE("numpy"             numpy        True                                        NUMPY_FOUND)

########################################################################
# Register component
########################################################################
include(GrComponent)
if(NOT CMAKE_CROSSCOMPILING)
    set(grc_python_deps
        PYTHON_MIN_VER_FOUND
        CHEETAH_FOUND
        LXML_FOUND
        PYGTK_FOUND
        NUMPY_FOUND
    )
endif(NOT CMAKE_CROSSCOMPILING)

GR_REGISTER_COMPONENT("gnuradio-companion" ENABLE_GRC
    ENABLE_GR_CORE
    ENABLE_PYTHON
    ${grc_python_deps}
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GRC)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_COMPONENT("grc"
    DISPLAY_NAME "GNU Radio Companion"
    DESCRIPTION  "Graphical flow graph designer"
    DEPENDS      "core_python"
)

########################################################################
# Create and install the grc conf file
########################################################################
file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${GRC_BLOCKS_DIR} blocksdir)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/grc.conf.in
    ${CMAKE_CURRENT_BINARY_DIR}/grc.conf
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/grc.conf
    DESTINATION ${GR_PKG_CONF_DIR}
    COMPONENT "grc"
)

GR_PYTHON_INSTALL(
    FILES __init__.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/grc
    COMPONENT "grc"
)

########################################################################
# Appens NSIS commands to set environment variables
########################################################################
if(WIN32)

file(TO_NATIVE_PATH ${GR_PKG_DOC_DIR} GR_DOC_DIR)
string(REPLACE "\\" "\\\\" GR_DOC_DIR ${GR_DOC_DIR})

file(TO_NATIVE_PATH ${GRC_BLOCKS_DIR} GRC_BLOCKS_PATH)
string(REPLACE "\\" "\\\\" GRC_BLOCKS_PATH ${GRC_BLOCKS_PATH})

file(TO_NATIVE_PATH ${GR_PYTHON_DIR} GR_PYTHON_POSTFIX)
string(REPLACE "\\" "\\\\" GR_PYTHON_POSTFIX ${GR_PYTHON_POSTFIX})

CPACK_SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
    WriteRegStr HKLM ${HLKM_ENV} \\\"GRC_BLOCKS_PATH\\\" \\\"$INSTDIR\\\\${GRC_BLOCKS_PATH}\\\"
    WriteRegStr HKLM \\\"SOFTWARE\\\\Python\\\\PythonCore\\\\2.7\\\\PythonPath\\\" \\\"gnuradio\\\" \\\"$INSTDIR\\\\${GR_PYTHON_POSTFIX}\\\"
")

CPACK_SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
    DeleteRegValue HKLM ${HLKM_ENV} \\\"GRC_BLOCKS_PATH\\\"
    DeleteRegValue HKLM \\\"SOFTWARE\\\\Python\\\\PythonCore\\\\2.7\\\\PythonPath\\\" \\\"gnuradio\\\"
")

endif(WIN32)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(base)
add_subdirectory(blocks)
add_subdirectory(freedesktop)
add_subdirectory(grc_gnuradio)
add_subdirectory(gui)
add_subdirectory(python)
add_subdirectory(scripts)

endif(ENABLE_GRC)
