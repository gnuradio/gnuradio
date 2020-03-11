# Copyright 2011,2016,2019 Free Software Foundation, Inc.
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

if(LIBCODEC2_FOUND)
  list(APPEND GR_SWIG_FLAGS "-DLIBCODEC2_FOUND")
endif(LIBCODEC2_FOUND)

if(LIBCODEC2_HAS_FREEDV_API)
  list(APPEND GR_SWIG_FLAGS "-DLIBCODEC2_HAS_FREEDV_API")
endif(LIBCODEC2_HAS_FREEDV_API)

if(LIBGSM_FOUND)
  list(APPEND GR_SWIG_FLAGS "-DLIBGSM_FOUND")
endif(LIBGSM_FOUND)

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/vocoder_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/vocoder)
set(GR_SWIG_DOCS_TARGET_DEPS runtime_swig_swig_doc)

set(GR_SWIG_LIBRARIES gnuradio-vocoder)

GR_SWIG_MAKE(vocoder_swig vocoder_swig.i)

GR_SWIG_INSTALL(
    TARGETS vocoder_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/vocoder
)

install(
    FILES
    vocoder_swig.i
    ${CMAKE_CURRENT_BINARY_DIR}/vocoder_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
)
