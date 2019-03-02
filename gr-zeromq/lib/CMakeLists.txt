# Copyright 2013 Free Software Foundation, Inc.
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
add_library(gnuradio-zeromq
  base_impl.cc
  pub_sink_impl.cc
  pub_msg_sink_impl.cc
  sub_source_impl.cc
  sub_msg_source_impl.cc
  pull_source_impl.cc
  pull_msg_source_impl.cc
  push_sink_impl.cc
  push_msg_sink_impl.cc
  rep_sink_impl.cc
  rep_msg_sink_impl.cc
  req_source_impl.cc
  req_msg_source_impl.cc
  tag_headers.cc
  )

target_link_libraries(gnuradio-zeromq PUBLIC
  gnuradio-runtime
  Boost::boost
  Boost::thread
  ZeroMQ::ZeroMQ
)

target_include_directories(gnuradio-zeromq
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  )

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-zeromq.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-zeromq.rc
    @ONLY)

    target_sources(gnuradio-zeromq PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-zeromq.rc
    )
endif(MSVC)



if(BUILD_SHARED_LIBS)
  GR_LIBRARY_FOO(gnuradio-zeromq ZeroMQ)
endif()
