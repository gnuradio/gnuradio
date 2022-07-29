# Copyright 2012-2013,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################
add_library(gnuradio-channels
  channel_model_impl.cc
  channel_model2_impl.cc
  dynamic_channel_model_impl.cc
  fading_model_impl.cc
  selective_fading_model_impl.cc
  selective_fading_model2_impl.cc
  flat_fader_impl.cc
  cfo_model_impl.cc
  sro_model_impl.cc
)

target_link_libraries(gnuradio-channels PUBLIC
    gnuradio-runtime
    gnuradio-filter
    gnuradio-analog
    gnuradio-blocks
)

if(ENABLE_COMMON_PCH)
  target_link_libraries(gnuradio-channels PRIVATE common-precompiled-headers)
endif()

target_include_directories(gnuradio-channels
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  )

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-channels.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-channels.rc
    @ONLY)

    target_sources(gnuradio-channels PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-channels.rc
    )
endif(MSVC)

if(BUILD_SHARED_LIBS)
  GR_LIBRARY_FOO(gnuradio-channels)
endif()
