# Copyright 2011,2013 Free Software Foundation, Inc.
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
    ENABLE_GNURADIO_RUNTIME
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
    DEPENDS      "runtime_python"
)

########################################################################
# Create and install the grc conf file
########################################################################
file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${GRC_BLOCKS_DIR} blocksdir)
if(CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    # linux binary installs: append blocks dir with prefix /usr/local
    set(blocksdir ${blocksdir}:/usr/local/${GRC_BLOCKS_DIR})
endif(CMAKE_INSTALL_PREFIX STREQUAL "/usr")

if(UNIX)
    find_program(GRC_XTERM_EXE
        NAMES xterminal-emulator gnome-terminal konsole xterm
        HINTS ENV PATH
        DOC "graphical terminal emulator used in GRC's no-gui-mode"
    )
    if(NOT GRC_XTERM_EXE)
        set(GRC_XTERM_EXE "")
    endif()
else()  # APPLE CYGWIN
    set(GRC_XTERM_EXE "xterm")
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/grc.conf.in
    ${CMAKE_CURRENT_BINARY_DIR}/grc.conf
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/grc.conf
    DESTINATION ${GR_PREFSDIR}
    COMPONENT "grc"
)

file(GLOB py_files "*.py")

GR_PYTHON_INSTALL(
    FILES ${py_files}
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
    #!include \\\"winmessages.nsh\\\"
    WriteRegStr HKLM ${HLKM_ENV} \\\"GRC_BLOCKS_PATH\\\" \\\"$INSTDIR\\\\${GRC_BLOCKS_PATH}\\\"
    SendMessage \\\${HWND_BROADCAST} \\\${WM_WININICHANGE} 0 \\\"STR:Environment\\\" /TIMEOUT=5000
")

CPACK_SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
    #!include \\\"winmessages.nsh\\\"
    DeleteRegValue HKLM ${HLKM_ENV} \\\"GRC_BLOCKS_PATH\\\"
    SendMessage \\\${HWND_BROADCAST} \\\${WM_WININICHANGE} 0 \\\"STR:Environment\\\" /TIMEOUT=5000
")

endif(WIN32)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(blocks)
add_subdirectory(gui)
add_subdirectory(core)
add_subdirectory(scripts)

endif(ENABLE_GRC)
