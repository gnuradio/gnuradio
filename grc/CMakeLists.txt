# Copyright 2011,2013,2017 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
include(GrPython)

message(STATUS "")


GR_PYTHON_CHECK_MODULE(
    DESC "PyYAML >= 3.11"
    MODULE yaml
    CHECK "yaml.__version__ >= '3.11'"
    VAR PYYAML_FOUND
)

GR_PYTHON_CHECK_MODULE(
    DESC "mako >= ${GR_MAKO_MIN_VERSION}"
    MODULE mako
    CHECK "LooseVersion(mako.__version__) >= LooseVersion('${GR_MAKO_MIN_VERSION}')"
    VAR MAKO_FOUND
)

GR_PYTHON_CHECK_MODULE_RAW(
    "pygobject >= 2.28.6"
    "import os; os.add_dll_directory('${GTK_BIN_DIR}');"
    PYGI_FOUND
)

GR_PYTHON_CHECK_MODULE_RAW(
    "Gtk (GI) >= 3.10.8"
    "import os; os.add_dll_directory('${GTK_BIN_DIR}');"
    GTK_GI_FOUND
)

GR_PYTHON_CHECK_MODULE_RAW(
    "Cairo (GI) >= 1.0"
    "import os; os.add_dll_directory('${GTK_BIN_DIR}');"  # Cairo 1.13.0
    CAIRO_GI_FOUND
)

GR_PYTHON_CHECK_MODULE_RAW(
    "PangoCairo (GI) >= 1.0"
    "import os; os.add_dll_directory('${GTK_BIN_DIR}');"  # pangocairo 1.36.3
    PANGOCAIRO_GI_FOUND
)

GR_PYTHON_CHECK_MODULE(
    DESC "numpy"
    MODULE numpy
    VAR NUMPY_FOUND
)

GR_PYTHON_CHECK_MODULE(
  DESC "jsonschema"
  MODULE jsonschema
  VAR JSONSCHEMA_FOUND
)

if(PYGI_FOUND)
    REGISTER_EXTERNAL_PYTHON_COMPONENT(gi)
    REGISTER_EXTERNAL_PYTHON_COMPONENT(cairo)
endif()
########################################################################
# Register component
########################################################################
include(GrComponent)
if(NOT CMAKE_CROSSCOMPILING)
    set(grc_python_deps
        PYYAML_FOUND
        MAKO_FOUND
        PYGI_FOUND
        GTK_GI_FOUND
        CAIRO_GI_FOUND
        PANGOCAIRO_GI_FOUND
        NUMPY_FOUND
    )
endif(NOT CMAKE_CROSSCOMPILING)

GR_REGISTER_COMPONENT(
    NAME "gnuradio-companion"
    VAR  ENABLE_GRC
    REQUIRED_COMPONNETS
        gnuradio-runtime
    REQUIRED_TPLS
        ENABLE_PYTHON
        ${grc_python_deps}
)

if(ENABLE_GRC)

  GR_REGISTER_COMPONENT(
    NAME "JSON/YAML config blocks"
    VAR ENABLE_JSONYAML_BLOCKS
    REQUIRED_COMPONENTS gnuradio-companion
    REQUIRED_TPLS       JSONSCHEMA_FOUND
    )

########################################################################
# Create and install the grc and grc-docs conf file
########################################################################
file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${GRC_BLOCKS_DIR} blocksdir)
file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${GRC_EXAMPLES_DIR} examplesdir)
if(CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    # linux binary installs: append blocks and examples dir with prefix /usr/local
    set(blocksdir ${blocksdir}:/usr/local/${GRC_BLOCKS_DIR})
    set(examplesdir ${examplesdir}:/usr/local/${GRC_EXAMPLES_DIR})
endif(CMAKE_INSTALL_PREFIX STREQUAL "/usr")

if(UNIX)
    find_program(GRC_XTERM_EXE
        NAMES x-terminal-emulator gnome-terminal konsole xfce4-terminal urxvt xterm foot
        HINTS ENV PATH
        DOC "graphical terminal emulator used in GRC's no-gui-mode"
    )
    if(NOT GRC_XTERM_EXE)
        set(GRC_XTERM_EXE "x-terminal-emulator")
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
    DESTINATION ${GR_PREFSDIR_relative_to_prefix}
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/00-grc-docs.conf.in
    ${CMAKE_CURRENT_BINARY_DIR}/00-grc-docs.conf
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/00-grc-docs.conf
    DESTINATION ${GR_PREFSDIR_relative_to_prefix}
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
    DIRECTORY gui_qt
    DESTINATION "${GR_PYTHON_DIR}/gnuradio/grc"
    FILES_MATCHING REGEX "\\.(py|yml|grc|mo|png|ui)$"
)

GR_PYTHON_INSTALL(
    DIRECTORY converter
    DESTINATION "${GR_PYTHON_DIR}/gnuradio/grc"
    FILES_MATCHING REGEX "\\.(py|dtd|grc|tmpl|png|mako)$"
)

GR_PYTHON_INSTALL(
    DIRECTORY workflows
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
add_subdirectory(workflows)
add_subdirectory(scripts)
add_subdirectory(tests)

endif(ENABLE_GRC)
