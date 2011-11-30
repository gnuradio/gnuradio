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
# Setup the include and linker paths
########################################################################
include_directories(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_VIDEO_SDL_INCLUDE_DIRS}
)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${SDL_INCLUDE_DIR})

########################################################################
# Setup library
########################################################################
list(APPEND gr_video_sdl_sources
    video_sdl_sink_uc.cc
    video_sdl_sink_s.cc
)

list(APPEND video_sdl_libs
    gnuradio-core
    ${Boost_LIBRARIES}
    ${SDL_LIBRARY}
)

add_library(gnuradio-video-sdl SHARED ${gr_video_sdl_sources})
target_link_libraries(gnuradio-video-sdl ${video_sdl_libs})
GR_LIBRARY_FOO(gnuradio-video-sdl RUNTIME_COMPONENT "video_sdl_runtime" DEVEL_COMPONENT "video_sdl_devel")

########################################################################
# Install public header files
########################################################################
install(FILES
    video_sdl_api.h
    video_sdl_sink_uc.h
    video_sdl_sink_s.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "video_sdl_devel"
)

########################################################################
# Setup swig generation
########################################################################
if(ENABLE_PYTHON)
include(GrPython)
include(GrSwig)

set(GR_SWIG_INCLUDE_DIRS
    ${GR_COMEDI_INCLUDE_DIRS}
    ${GNURADIO_CORE_SWIG_INCLUDE_DIRS}
)

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/video_sdl_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

set(GR_SWIG_LIBRARIES gnuradio-video-sdl)

GR_SWIG_MAKE(video_sdl video_sdl.i)

GR_SWIG_INSTALL(
    TARGETS video_sdl
    DESTINATION ${GR_PYTHON_DIR}/gnuradio
    COMPONENT "video_sdl_python"
)

install(
    FILES video_sdl.i
    ${CMAKE_CURRENT_BINARY_DIR}/video_sdl_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "video_sdl_swig"
)

endif(ENABLE_PYTHON)

########################################################################
# Handle the unit tests
########################################################################
if(ENABLE_TESTING AND ENABLE_PYTHON)
include(GrTest)
file(GLOB py_qa_test_files "qa_*.py")
foreach(py_qa_test_file ${py_qa_test_files})
    get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
    set(GR_TEST_PYTHON_DIRS
        ${CMAKE_BINARY_DIR}/gnuradio-core/src/python
        ${CMAKE_BINARY_DIR}/gnuradio-core/src/lib/swig
        ${CMAKE_BINARY_DIR}/gr-video-sdl/src
    )
    set(GR_TEST_TARGET_DEPS gruel gnuradio-core gnuradio-video-sdl)
    GR_ADD_TEST(${py_qa_test_name} ${PYTHON_EXECUTABLE} ${py_qa_test_file})
endforeach(py_qa_test_file)
endif(ENABLE_TESTING AND ENABLE_PYTHON)
