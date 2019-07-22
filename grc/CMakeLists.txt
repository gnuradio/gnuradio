# Copyright 2011,2013,2017 Free Software Foundation, Inc.
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

message(STATUS "")

GR_PYTHON_CHECK_MODULE_RAW(
    "python2 >= 2.7.6 or python3 >= 3.6.5"
    "import sys; \
     requirement = (3, 6, 5) if sys.version_info.major >= 3 else (2, 7, 6); \
     assert sys.version_info[:3] >= requirement"
    PYTHON_MIN_VER_FOUND
)

GR_PYTHON_CHECK_MODULE_RAW(
    "PyYAML >= 3.10"
    "import yaml; assert yaml.__version__ >= '3.11'"
    PYYAML_FOUND
)

GR_PYTHON_CHECK_MODULE_RAW(
    "mako >= 0.9.1"
    "import mako; assert mako.__version__ >= '0.9.1'"
    MAKO_FOUND
)

GR_PYTHON_CHECK_MODULE_RAW(
    "pygobject >= 2.28.6"
    "import gi; assert gi.version_info >= (2, 28, 6)"
    PYGI_FOUND
)

GR_PYTHON_CHECK_MODULE_RAW(
    "Gtk (GI) >= 3.10.8"
    "import gi; gi.require_version('Gtk', '3.0'); \
     from gi.repository import Gtk; Gtk.check_version(3, 10, 8)"
    GTK_GI_FOUND
)

GR_PYTHON_CHECK_MODULE_RAW(
    "Cairo (GI) >= 1.0"
    "import gi; gi.require_foreign('cairo', 'Context')"  # Cairo 1.13.0
    CAIRO_GI_FOUND
)

GR_PYTHON_CHECK_MODULE_RAW(
    "PangoCairo (GI) >= 1.0"
    "import gi; gi.require_version('PangoCairo', '1.0')"  # pangocairo 1.36.3
    PANGOCAIRO_GI_FOUND
)

GR_PYTHON_CHECK_MODULE_RAW(
    "numpy"
    "import numpy"
    NUMPY_FOUND
)

########################################################################
# Register component
########################################################################
include(GrComponent)
if(NOT CMAKE_CROSSCOMPILING)
    set(grc_python_deps
        PYTHON_MIN_VER_FOUND
        PYYAML_FOUND
        MAKO_FOUND
        PYGI_FOUND
        GTK_GI_FOUND
        CAIRO_GI_FOUND
        PANGOCAIRO_GI_FOUND
        NUMPY_FOUND
    )
endif(NOT CMAKE_CROSSCOMPILING)

GR_REGISTER_COMPONENT("gnuradio-companion" ENABLE_GRC
    ENABLE_GNURADIO_RUNTIME
    ENABLE_PYTHON
    ${grc_python_deps}
)

if(ENABLE_GRC)

########################################################################
# Create and install the grc and grc-docs conf file
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
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/00-grc-docs.conf.in
    ${CMAKE_CURRENT_BINARY_DIR}/00-grc-docs.conf
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/00-grc-docs.conf
    DESTINATION ${GR_PREFSDIR}
)

########################################################################
# Install (+ compile) python sources and data files
########################################################################
file(GLOB py_files "*.py")
GR_PYTHON_INSTALL(
    FILES ${py_files}
    DESTINATION "${GR_PYTHON_DIR}/gnuradio/grc"
)

GR_PYTHON_INSTALL(
    DIRECTORY core
    DESTINATION "${GR_PYTHON_DIR}/gnuradio/grc"
    FILES_MATCHING REGEX "\\.(py|dtd|grc|tmpl|png|mako)$"
)

GR_PYTHON_INSTALL(
    DIRECTORY gui
    DESTINATION "${GR_PYTHON_DIR}/gnuradio/grc"
    FILES_MATCHING REGEX "\\.(py|dtd|grc|tmpl|png|mako)$"
)

GR_PYTHON_INSTALL(
    DIRECTORY converter
    DESTINATION "${GR_PYTHON_DIR}/gnuradio/grc"
    FILES_MATCHING REGEX "\\.(py|dtd|grc|tmpl|png|mako)$"
)

########################################################################
# Append NSIS commands to set environment variables
########################################################################
if(WIN32)

file(TO_NATIVE_PATH ${GR_PKG_DOC_DIR} GR_DOC_DIR)
string(REPLACE "\\" "\\\\" GR_DOC_DIR ${GR_DOC_DIR})

file(TO_NATIVE_PATH ${GRC_BLOCKS_DIR} GRC_BLOCKS_PATH)
string(REPLACE "\\" "\\\\" GRC_BLOCKS_PATH ${GRC_BLOCKS_PATH})

file(TO_NATIVE_PATH ${GR_PYTHON_DIR} GR_PYTHON_POSTFIX)
string(REPLACE "\\" "\\\\" GR_PYTHON_POSTFIX ${GR_PYTHON_POSTFIX})

endif(WIN32)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(blocks)
add_subdirectory(scripts)

endif(ENABLE_GRC)
