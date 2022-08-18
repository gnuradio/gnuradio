# Copyright 2021 Jacob Gilbert
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################
add_library(
    gnuradio-pdu
    add_system_time_impl.cc
    pdu_filter_impl.cc
    pdu_remove_impl.cc
    pdu_set_impl.cc
    pdu_split_impl.cc
    pdu_to_stream_impl.cc
    pdu_to_tagged_stream_impl.cc
    random_pdu_impl.cc
    tags_to_pdu_impl.cc
    tagged_stream_to_pdu_impl.cc
    take_skip_to_pdu_impl.cc
    time_delta_impl.cc)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-pdu.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pdu.rc @ONLY)

    target_sources(gnuradio-pdu PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pdu.rc)
endif(MSVC)

target_link_libraries(gnuradio-pdu PUBLIC gnuradio-runtime)

if(ENABLE_COMMON_PCH)
    target_link_libraries(gnuradio-pdu PRIVATE common-precompiled-headers)
endif()

target_include_directories(
    gnuradio-pdu
    PUBLIC $<INSTALL_INTERFACE:include>
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)

if(BUILD_SHARED_LIBS)
    gr_library_foo(gnuradio-pdu)
endif()
