# Copyright 2010-2012,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup swig generation
########################################################################
include(GrPython)
include(GrSwig)

add_definitions(-DENABLE_PYTHON)
list(APPEND GR_SWIG_FLAGS "-DENABLE_PYTHON")

set(GR_SWIG_INCLUDE_DIRS $<TARGET_PROPERTY:runtime_swig,INCLUDE_DIRECTORIES>)
set(GR_SWIG_TARGET_DEPS runtime_swig)

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/qtgui_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/qtgui)
set(GR_SWIG_DOCS_TARGET_DEPS runtime_swig_swig_doc)

set(GR_SWIG_LIBRARIES gnuradio-qtgui)

GR_SWIG_MAKE(qtgui_swig qtgui_swig.i)

GR_SWIG_INSTALL(
    TARGETS qtgui_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/qtgui
)

install(FILES
    qtgui_swig.i
    ${CMAKE_CURRENT_BINARY_DIR}/qtgui_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
)
