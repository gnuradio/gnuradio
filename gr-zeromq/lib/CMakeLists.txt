# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

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
  ZeroMQ::ZeroMQ
)

if(ENABLE_COMMON_PCH)
  target_link_libraries(gnuradio-zeromq PRIVATE common-precompiled-headers)
endif()

target_include_directories(gnuradio-zeromq
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  )

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

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
