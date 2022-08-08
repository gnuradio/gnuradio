# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

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

if(ENABLE_COMMON_PCH)
  target_link_libraries(gnuradio-video-sdl PRIVATE common-precompiled-headers)
endif()

target_include_directories(gnuradio-video-sdl
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  ${SDL_INCLUDE_DIR}
  )


#Add Windows DLL resource file if using MSVC
IF(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

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
