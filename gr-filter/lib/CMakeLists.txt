# Copyright (C) 2012-2014,2017-2019,2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################
add_library(
    gnuradio-filter
    dc_blocker_cc_impl.cc
    dc_blocker_ff_impl.cc
    fft_filter.cc
    fft_filter_ccc_impl.cc
    fft_filter_ccf_impl.cc
    fft_filter_fff_impl.cc
    filter_delay_fc_impl.cc
    filterbank.cc
    filterbank_vcvcf_impl.cc
    fir_filter.cc
    fir_filter_blk_impl.cc
    fir_filter_with_buffer.cc
    firdes.cc
    freq_xlating_fir_filter_impl.cc
    hilbert_fc_impl.cc
    iir_filter.cc
    iir_filter_ccc_impl.cc
    iir_filter_ccd_impl.cc
    iir_filter_ccf_impl.cc
    iir_filter_ccz_impl.cc
    iir_filter_ffd_impl.cc
    interp_fir_filter_impl.cc
    ival_decimator_impl.cc
    mmse_fir_interpolator.cc
    mmse_interp_differentiator_cc.cc
    mmse_interp_differentiator_ff.cc
    mmse_resampler_impl.cc
    pfb_arb_resampler.cc
    pfb_arb_resampler_ccc_impl.cc
    pfb_arb_resampler_ccf_impl.cc
    pfb_arb_resampler_fff_impl.cc
    pfb_channelizer_ccf_impl.cc
    pfb_decimator_ccf_impl.cc
    pfb_interpolator_ccf_impl.cc
    pfb_synthesizer_ccf_impl.cc
    pm_remez.cc
    polyphase_filterbank.cc
    rational_resampler_impl.cc
    single_pole_iir_filter_cc_impl.cc
    single_pole_iir_filter_ff_impl.cc
)

target_link_libraries(
    gnuradio-filter
    PUBLIC gnuradio-runtime gnuradio-fft Volk::volk
    PRIVATE gnuradio-blocks)

target_include_directories(
    gnuradio-filter PUBLIC $<INSTALL_INTERFACE:include>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-filter.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-filter.rc @ONLY)

    target_sources(gnuradio-filter PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-filter.rc)
endif(MSVC)

gr_library_foo(gnuradio-filter)

########################################################################
# QA C++ Code for gr-filter
########################################################################
if(ENABLE_TESTING)
    include(GrTest)

    list(
        APPEND
        test_gr_filter_sources
        qa_firdes.cc
        qa_fir_filter_with_buffer.cc
        qa_mmse_fir_interpolator_cc.cc
        qa_mmse_fir_interpolator_ff.cc
        qa_mmse_interp_differentiator_cc.cc
        qa_mmse_interp_differentiator_ff.cc)
    list(APPEND GR_TEST_TARGET_DEPS gnuradio-filter gnuradio-fft)

    foreach(qa_file ${test_gr_filter_sources})
        gr_add_cpp_test("filter_${qa_file}" ${CMAKE_CURRENT_SOURCE_DIR}/${qa_file})
    endforeach(qa_file)
endif(ENABLE_TESTING)
