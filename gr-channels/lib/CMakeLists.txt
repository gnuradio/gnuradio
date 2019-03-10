# Copyright 2012-2013,2019 Free Software Foundation, Inc.
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
    Boost::boost
)

target_include_directories(gnuradio-channels
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  )

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

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
