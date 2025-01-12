# Copyright 2011,2013,2016,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################
add_library(
    gnuradio-vocoder
    alaw_decode_bs_impl.cc
    alaw_encode_sb_impl.cc
    cvsd_decode_bs_impl.cc
    cvsd_encode_sb_impl.cc
    g721_decode_bs_impl.cc
    g721_encode_sb_impl.cc
    g723_24_decode_bs_impl.cc
    g723_24_encode_sb_impl.cc
    g723_40_decode_bs_impl.cc
    g723_40_encode_sb_impl.cc
    ulaw_decode_bs_impl.cc
    ulaw_encode_sb_impl.cc)
target_include_directories(
    gnuradio-vocoder
    PUBLIC $<INSTALL_INTERFACE:include>
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_link_libraries(gnuradio-vocoder PUBLIC gnuradio-runtime)

if(LIBCODEC2_FOUND)
    target_sources(gnuradio-vocoder PRIVATE codec2.cc codec2_decode_ps_impl.cc
                                            codec2_encode_sp_impl.cc)
endif(LIBCODEC2_FOUND)
if(LIBCODEC2_HAS_FREEDV_API)
    target_sources(gnuradio-vocoder PRIVATE freedv_api.cc freedv_rx_ss_impl.cc
                                            freedv_tx_ss_impl.cc)
endif(LIBCODEC2_HAS_FREEDV_API)
if(LIBGSM_FOUND)
    target_sources(gnuradio-vocoder PRIVATE gsm_fr_decode_ps_impl.cc
                                            gsm_fr_encode_sp_impl.cc)
endif(LIBGSM_FOUND)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-vocoder.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-vocoder.rc @ONLY)

    list(APPEND gr_uhd_sources ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-vocoder.rc)
endif(MSVC)

########################################################################
# Include subdirs rather to populate to the sources lists.
########################################################################
target_sources(
    gnuradio-vocoder
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/g7xx/g711.c
            ${CMAKE_CURRENT_SOURCE_DIR}/g7xx/g72x.c
            ${CMAKE_CURRENT_SOURCE_DIR}/g7xx/g721.c
            ${CMAKE_CURRENT_SOURCE_DIR}/g7xx/g723_24.c
            ${CMAKE_CURRENT_SOURCE_DIR}/g7xx/g723_40.c)

set(external_deps "")
if(LIBCODEC2_LIBRARIES)
    target_link_libraries(gnuradio-vocoder PUBLIC CODEC2::CODEC2)
    list(APPEND external_deps Codec2)
endif(LIBCODEC2_LIBRARIES)

if(LIBGSM_LIBRARIES)
    target_link_libraries(gnuradio-vocoder PRIVATE GSM::GSM)
endif(LIBGSM_LIBRARIES)

if(BUILD_SHARED_LIBS)
    gr_library_foo(gnuradio-vocoder ${external_deps})
endif()
