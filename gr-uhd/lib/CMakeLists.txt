# Copyright 2011-2013,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################
set(LIB_GR_UHD_SOURCES amsg_source_impl.cc usrp_block_impl.cc usrp_sink_impl.cc
                       usrp_source_impl.cc)

if(ENABLE_UHD_RFNOC)
    list(
        APPEND
        LIB_GR_UHD_SOURCES
        rfnoc_block.cc
        rfnoc_block_generic_impl.cc
        rfnoc_ddc_impl.cc
        rfnoc_duc_impl.cc
        rfnoc_graph_impl.cc
        rfnoc_rx_radio_impl.cc
        rfnoc_rx_streamer_impl.cc
        rfnoc_tx_radio_impl.cc
        rfnoc_tx_streamer_impl.cc)
endif(ENABLE_UHD_RFNOC)

add_library(gnuradio-uhd ${LIB_GR_UHD_SOURCES})

target_link_libraries(gnuradio-uhd gnuradio-runtime UHD::UHD)

target_include_directories(
    gnuradio-uhd PUBLIC $<INSTALL_INTERFACE:include>
                        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-uhd.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-uhd.rc @ONLY)

    target_sources(gnuradio-uhd PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-uhd.rc)
endif(MSVC)

if(BUILD_SHARED_LIBS)
    gr_library_foo(gnuradio-uhd UHD)
endif()
