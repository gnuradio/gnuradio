# Copyright 2012 Free Software Foundation, Inc.
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
# Setup swig generation
########################################################################
include(GrPython)
include(GrSwig)

set(GR_SWIG_INCLUDE_DIRS
    ${GR_VIDEO_SDL_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_SWIG_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

set(GR_SWIG_LIBRARIES gnuradio-video-sdl)

if(ENABLE_GR_CTRLPORT)
  list(APPEND GR_SWIG_FLAGS "-DGR_CTRLPORT")
endif(ENABLE_GR_CTRLPORT)

# Setup swig docs to depend on includes and pull in from build directory
set(GR_SWIG_TARGET_DEPS video_sdl_generated_includes)
set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/video_sdl_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/video_sdl)
set(GR_SWIG_DOCS_TARGET_DEPS runtime_swig_swig_doc)

set(GR_SWIG_LIBRARIES gnuradio-video-sdl)

GR_SWIG_MAKE(video_sdl_swig video_sdl_swig.i)

GR_SWIG_INSTALL(
    TARGETS video_sdl_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/video_sdl
    COMPONENT "video_sdl_python"
)

install(
    FILES
    video_sdl_swig.i
    ${CMAKE_CURRENT_BINARY_DIR}/video_sdl_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "video_sdl_swig"
)
