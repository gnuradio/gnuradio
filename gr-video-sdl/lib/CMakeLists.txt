# Copyright 2011,2012 Free Software Foundation, Inc.
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
    ${GR_VIDEO_SDL_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${LOG4CXX_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${SDL_INCLUDE_DIR}
)

link_directories(${LOG4CXX_LIBRARY_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
  include_directories(${ICE_INCLUDE_DIR})
endif(ENABLE_GR_CTRLPORT)

########################################################################
# Setup library
########################################################################
list(APPEND video_sdl_sources
  sink_s_impl.cc
  sink_uc_impl.cc
)

list(APPEND video_sdl_libs
    gnuradio-runtime
    ${Boost_LIBRARIES}
    ${SDL_LIBRARY}
)

#Add Windows DLL resource file if using MSVC
IF(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-video-sdl.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-video-sdl.rc
    @ONLY)

    list(APPEND video_sdl_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-video-sdl.rc
    )
ENDIF(MSVC)

add_library(gnuradio-video-sdl SHARED ${video_sdl_sources})
target_link_libraries(gnuradio-video-sdl ${video_sdl_libs})
GR_LIBRARY_FOO(gnuradio-video-sdl RUNTIME_COMPONENT "video_sdl_runtime" DEVEL_COMPONENT "video_sdl_devel")
add_dependencies(gnuradio-video-sdl 
  gnuradio-runtime)
