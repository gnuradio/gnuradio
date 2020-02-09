# Copyright 2011,2019 Free Software Foundation, Inc.
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

set(GR_SWIG_FLAGS -DGR_HAVE_UHD) #needed to parse uhd_swig.i

set(GR_SWIG_INCLUDE_DIRS $<TARGET_PROPERTY:runtime_swig,INCLUDE_DIRECTORIES>)
set(GR_SWIG_TARGET_DEPS runtime_swig)

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/uhd_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/uhd)
set(GR_SWIG_DOCS_TARGET_DEPS runtime_swig_swig_doc)

link_directories(${UHD_LIBRARY_DIRS})
set(GR_SWIG_LIBRARIES gnuradio-uhd ${UHD_LIBRARIES})

GR_SWIG_MAKE(uhd_swig uhd_swig.i)

GR_SWIG_INSTALL(
    TARGETS uhd_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/uhd
)

install(
    FILES uhd_swig.i
    ${CMAKE_CURRENT_BINARY_DIR}/uhd_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
)
