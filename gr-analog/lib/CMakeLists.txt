# Copyright 2012-2014,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################

add_library(
    gnuradio-analog
    cpm.cc
    squelch_base_cc_impl.cc
    squelch_base_ff_impl.cc
    agc_cc_impl.cc
    agc_ff_impl.cc
    agc2_cc_impl.cc
    agc2_ff_impl.cc
    agc3_cc_impl.cc
    cpfsk_bc_impl.cc
    ctcss_squelch_ff_impl.cc
    dpll_bb_impl.cc
    fastnoise_source_impl.cc
    feedforward_agc_cc_impl.cc
    fmdet_cf_impl.cc
    frequency_modulator_fc_impl.cc
    noise_source_impl.cc
    phase_modulator_fc_impl.cc
    pll_carriertracking_cc_impl.cc
    pll_freqdet_cf_impl.cc
    pll_refout_cc_impl.cc
    probe_avg_mag_sqrd_c_impl.cc
    probe_avg_mag_sqrd_cf_impl.cc
    probe_avg_mag_sqrd_f_impl.cc
    pwr_squelch_cc_impl.cc
    pwr_squelch_ff_impl.cc
    quadrature_demod_cf_impl.cc
    rail_ff_impl.cc
    random_uniform_source_impl.cc
    sig_source_impl.cc
    simple_squelch_cc_impl.cc)

target_link_libraries(
    gnuradio-analog
    PUBLIC gnuradio-runtime gnuradio-blocks
    PRIVATE gnuradio-filter)

target_include_directories(
    gnuradio-analog PUBLIC $<INSTALL_INTERFACE:include>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-analog.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-analog.rc @ONLY)

    target_sources(gnuradio-analog PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-analog.rc)

endif(MSVC)

if(BUILD_SHARED_LIBS)
    gr_library_foo(gnuradio-analog)
endif()

########################################################################
# QA C++ Code for gr-analog
########################################################################
if(ENABLE_TESTING)
    include(GrTest)

    list(APPEND test_gr_analog_sources)
    list(APPEND GR_TEST_TARGET_DEPS gnuradio-analog)

    foreach(qa_file ${test_gr_analog_sources})
        gr_add_cpp_test("analog_${qa_file}" ${CMAKE_CURRENT_SOURCE_DIR}/${qa_file})
    endforeach(qa_file)
endif(ENABLE_TESTING)
