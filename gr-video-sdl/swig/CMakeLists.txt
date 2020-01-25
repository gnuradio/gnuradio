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

set(GR_SWIG_LIBRARIES gnuradio-video-sdl)

# Setup swig docs to depend on includes and pull in from build directory
set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/video_sdl_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/video_sdl)
set(GR_SWIG_DOCS_TARGET_DEPS runtime_swig_swig_doc)

set(GR_SWIG_LIBRARIES gnuradio-video-sdl)

GR_SWIG_MAKE(video_sdl_swig video_sdl_swig.i)

GR_SWIG_INSTALL(
    TARGETS video_sdl_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/video_sdl
)

install(
    FILES
    video_sdl_swig.i
    ${CMAKE_CURRENT_BINARY_DIR}/video_sdl_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
)
