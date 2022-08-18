# Copyright 2012,2014,2018-2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################
add_library(
    gnuradio-trellis
    base.cc
    calc_metric.cc
    core_algorithms.cc
    encoder_impl.cc
    fsm.cc
    interleaver.cc
    quicksort_index.cc
    constellation_metrics_cf_impl.cc
    metrics_impl.cc
    pccc_decoder_blk_impl.cc
    pccc_encoder_impl.cc
    pccc_decoder_combined_blk_impl.cc
    permutation_impl.cc
    sccc_decoder_blk_impl.cc
    sccc_decoder_combined_blk_impl.cc
    sccc_encoder_impl.cc
    siso_f_impl.cc
    siso_combined_f_impl.cc
    viterbi_impl.cc
    viterbi_combined_impl.cc)

target_link_libraries(gnuradio-trellis PUBLIC gnuradio-runtime gnuradio-digital)

if(ENABLE_COMMON_PCH)
    target_link_libraries(gnuradio-trellis PRIVATE common-precompiled-headers)
endif()

target_include_directories(
    gnuradio-trellis PUBLIC $<INSTALL_INTERFACE:include>
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-trellis.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-trellis.rc @ONLY)

    target_sources(gnuradio-trellis
                   PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-trellis.rc)
endif(MSVC)

if(BUILD_SHARED_LIBS)
    gr_library_foo(gnuradio-trellis)
endif()
