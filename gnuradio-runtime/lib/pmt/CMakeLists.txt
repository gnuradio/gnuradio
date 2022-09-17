# Copyright 2010-2013,2018-2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

add_library(
    gnuradio-pmt
    ${CMAKE_CURRENT_SOURCE_DIR}/pmt_unv.cc ${CMAKE_CURRENT_SOURCE_DIR}/pmt.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/pmt_io.cc ${CMAKE_CURRENT_SOURCE_DIR}/pmt_pool.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/pmt_serialize.cc)

target_link_libraries(
    gnuradio-pmt
    PUBLIC Boost::boost
    PRIVATE Volk::volk)

target_include_directories(
    gnuradio-pmt
    PUBLIC $<INSTALL_INTERFACE:include>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-pmt.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pmt.rc @ONLY)

    target_sources(gnuradio-pmt PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pmt.rc)
endif(MSVC)

if(BUILD_SHARED_LIBS)
    gr_library_foo(gnuradio-pmt)
endif()
