# Copyright 2014,2015,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GR_DTV_INCLUDE_DIRS}
    ${GR_FILTER_INCLUDE_DIRS}
    ${GR_ANALOG_INCLUDE_DIRS}
    ${GR_FEC_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${VOLK_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
endif(ENABLE_GR_CTRLPORT)

########################################################################
# Setup library
########################################################################
list(APPEND dtv_sources
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
  catv/catv_trellis_enc_bb_impl.cc
)

list(APPEND dtv_libs
    gnuradio-runtime
    gnuradio-analog
    gnuradio-filter
    gnuradio-fec
    ${VOLK_LIBRARIES}
)

include (CheckCCompilerFlag)
if (MSVC)
	# 64-bit MSVC always supports SSE2 
	if (CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(SSE2_SUPPORTED true)
	else ()
		CHECK_C_COMPILER_FLAG ("/arch:SSE2" SSE2_SUPPORTED)
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
else ()
	CHECK_C_COMPILER_FLAG ("-msse2" SSE2_SUPPORTED)
endif(MSVC)

if(SSE2_SUPPORTED)
	if (NOT MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
	else ()
		if (CMAKE_SIZEOF_VOID_P EQUAL 4)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
		endif ()
	endif ()
    ADD_DEFINITIONS(-DDTV_SSE2)
endif(SSE2_SUPPORTED)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-dtv.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-dtv.rc
    @ONLY)

    list(APPEND dtv_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-dtv.rc
    )
endif(MSVC)

add_library(gnuradio-dtv SHARED ${dtv_sources})
target_link_libraries(gnuradio-dtv ${dtv_libs})
GR_LIBRARY_FOO(gnuradio-dtv RUNTIME_COMPONENT "dtv_runtime" DEVEL_COMPONENT "dtv_devel")
