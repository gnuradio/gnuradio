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
# Setup library
########################################################################
add_library(gnuradio-video-sdl
  sink_s_impl.cc
  sink_uc_impl.cc
)

target_link_libraries(gnuradio-video-sdl PUBLIC
    gnuradio-runtime
    ${SDL_LIBRARY}
)

target_include_directories(gnuradio-video-sdl
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  ${SDL_INCLUDE_DIR}
  )


#Add Windows DLL resource file if using MSVC
IF(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-video-sdl.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-video-sdl.rc
    @ONLY)

    target_sources(gnuradio-video-sdl PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-video-sdl.rc
    )
ENDIF(MSVC)

if(BUILD_SHARED_LIBS)
  GR_LIBRARY_FOO(gnuradio-video-sdl)
endif()
