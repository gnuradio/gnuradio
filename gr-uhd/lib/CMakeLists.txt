# Copyright 2011-2013,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################
add_library(gnuradio-uhd
    usrp_block_impl.cc
    usrp_source_impl.cc
    usrp_sink_impl.cc
    amsg_source_impl.cc
)

target_link_libraries(gnuradio-uhd
  gnuradio-runtime
  UHD::UHD
)

target_include_directories(gnuradio-uhd
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  )



#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-uhd.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-uhd.rc
    @ONLY)

    target_sources(gnuradio-uhd PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-uhd.rc
    )
endif(MSVC)

if(BUILD_SHARED_LIBS)
  GR_LIBRARY_FOO(gnuradio-uhd UHD)
endif()
