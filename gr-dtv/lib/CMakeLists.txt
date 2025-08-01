# Copyright 2014,2015,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################
add_library(
    gnuradio-dtv
    atsc/atsc_plinfo.cc
    atsc/atsc_deinterleaver_impl.cc
    atsc/atsc_depad_impl.cc
    atsc/atsc_derandomizer_impl.cc
    atsc/atsc_equalizer_impl.cc
    atsc/atsc_fpll_impl.cc
    atsc/atsc_fs_checker_impl.cc
    atsc/atsc_randomize.cc
    atsc/atsc_rs_decoder_impl.cc
    atsc/atsc_sync_impl.cc
    atsc/atsc_fake_single_viterbi.cc
    atsc/atsc_single_viterbi.cc
    atsc/atsc_viterbi_decoder_impl.cc
    atsc/atsc_pad_impl.cc
    atsc/atsc_randomizer_impl.cc
    atsc/atsc_rs_encoder_impl.cc
    atsc/atsc_interleaver_impl.cc
    atsc/atsc_basic_trellis_encoder.cc
    atsc/atsc_trellis_encoder_impl.cc
    atsc/atsc_field_sync_mux_impl.cc
    dvb/dvb_bbheader_bb_impl.cc
    dvb/dvb_bbscrambler_bb_impl.cc
    dvb/dvb_bch_bb_impl.cc
    dvb/dvb_ldpc_bb_impl.cc
    dvbt2/dvbt2_interleaver_bb_impl.cc
    dvbt2/dvbt2_modulator_bc_impl.cc
    dvbt2/dvbt2_cellinterleaver_cc_impl.cc
    dvbt2/dvbt2_framemapper_cc_impl.cc
    dvbt2/dvbt2_freqinterleaver_cc_impl.cc
    dvbt2/dvbt2_pilotgenerator_cc_impl.cc
    dvbt2/dvbt2_paprtr_cc_impl.cc
    dvbt2/dvbt2_p1insertion_cc_impl.cc
    dvbt2/dvbt2_miso_cc_impl.cc
    dvbs2/dvbs2_interleaver_bb_impl.cc
    dvbs2/dvbs2_modulator_bc_impl.cc
    dvbs2/dvbs2_physical_cc_impl.cc
    dvbt/dvbt_energy_dispersal_impl.cc
    dvbt/dvbt_reed_solomon_enc_impl.cc
    dvbt/dvbt_convolutional_interleaver_impl.cc
    dvbt/dvbt_configure.cc
    dvbt/dvbt_inner_coder_impl.cc
    dvbt/dvbt_bit_inner_interleaver_impl.cc
    dvbt/dvbt_symbol_inner_interleaver_impl.cc
    dvbt/dvbt_map_impl.cc
    dvbt/dvbt_reference_signals_impl.cc
    dvbt/dvbt_ofdm_sym_acquisition_impl.cc
    dvbt/dvbt_demod_reference_signals_impl.cc
    dvbt/dvbt_demap_impl.cc
    dvbt/dvbt_bit_inner_deinterleaver_impl.cc
    dvbt/dvbt_viterbi_decoder_impl.cc
    dvbt/dvbt_convolutional_deinterleaver_impl.cc
    dvbt/dvbt_reed_solomon_dec_impl.cc
    dvbt/dvbt_energy_descramble_impl.cc
    catv/catv_transport_framing_enc_bb_impl.cc
    catv/catv_reed_solomon_enc_bb_impl.cc
    catv/catv_randomizer_bb_impl.cc
    catv/catv_frame_sync_enc_bb_impl.cc
    catv/catv_trellis_enc_bb_impl.cc)

target_link_libraries(
    gnuradio-dtv
    PUBLIC gnuradio-runtime
    PRIVATE gnuradio-analog gnuradio-filter gnuradio-fec Volk::volk)

target_include_directories(
    gnuradio-dtv PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
                        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)

include(CheckCCompilerFlag)
if(MSVC)
    # 64-bit MSVC always supports SSE2
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SSE2_SUPPORTED true)
    else()
        check_c_compiler_flag("/arch:SSE2" SSE2_SUPPORTED)
    endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
else()
    # The "correct" way to test this would be to run
    # check_c_compiler_flag("-msse2")
    # ...but that's not reliable, in particular, on Clang and ARM, it will
    # return TRUE when it shouldn't. Hence, we compile this snippet
    # manually.
    check_c_source_compiles(
        "#include <emmintrin.h>\nint main(){__m128i m0, m1, m2; m0 = _mm_add_epi8(m1, m2);}"
        SSE2_SUPPORTED)
endif(MSVC)

if(SSE2_SUPPORTED)
    if(NOT MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
    else()
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
        endif()
    endif()
    target_compile_definitions(gnuradio-dtv PRIVATE -DDTV_SSE2)
endif(SSE2_SUPPORTED)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-dtv.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-dtv.rc @ONLY)

    target_sources(gnuradio-dtv PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-dtv.rc)
endif(MSVC)

if(BUILD_SHARED_LIBS)
    gr_library_foo(gnuradio-dtv)
endif()
