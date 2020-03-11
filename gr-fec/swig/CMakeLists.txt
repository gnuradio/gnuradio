# Copyright 2012,2019 Free Software Foundation, Inc.
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

set(GR_SWIG_INCLUDE_DIRS $<TARGET_PROPERTY:runtime_swig,INCLUDE_DIRECTORIES>)
set(GR_SWIG_TARGET_DEPS runtime_swig)

if(GSL_FOUND)
  list(APPEND GR_SWIG_FLAGS "-DGSL_FOUND")
endif(GSL_FOUND)

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/fec_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/fec)
set(GR_SWIG_DOCS_TARGET_DEPS runtime_swig_swig_doc)
set(GR_SWIG_LIBRARIES gnuradio-fec ${GSL_LDFLAGS})

GR_SWIG_MAKE(fec_swig fec_swig.i)

GR_SWIG_INSTALL(
    TARGETS fec_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/fec
)

install(
    FILES
    fec_swig.i
    ${CMAKE_CURRENT_BINARY_DIR}/fec_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
)
