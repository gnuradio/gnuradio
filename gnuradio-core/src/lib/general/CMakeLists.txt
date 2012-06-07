# Copyright 2010-2012 Free Software Foundation, Inc.
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
# This file included, use CMake directory variables
########################################################################

########################################################################
# Handle the generated sine table
########################################################################
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/sine_table.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gen_sine_table.py
    COMMAND ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/gen_sine_table.py >
        ${CMAKE_CURRENT_BINARY_DIR}/sine_table.h
)

include(AddFileDependencies)
ADD_FILE_DEPENDENCIES(${CMAKE_CURRENT_SOURCE_DIR}/gr_fxpt.cc
    ${CMAKE_CURRENT_BINARY_DIR}/sine_table.h
)

########################################################################
# Handle the generated constants
########################################################################
execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
    "import time;print time.strftime('%a, %d %b %Y %H:%M:%S', time.gmtime())"
    OUTPUT_VARIABLE BUILD_DATE OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Loading build date ${BUILD_DATE} into gr_constants...")

message(STATUS "Loading version ${VERSION} into gr_constants...")

file(TO_NATIVE_PATH "${CMAKE_INSTALL_PREFIX}/${GR_CONF_DIR}"     SYSCONFDIR)
file(TO_NATIVE_PATH "${CMAKE_INSTALL_PREFIX}/${GR_PKG_CONF_DIR}" GR_PREFSDIR)

#double escape for windows backslash path separators
string(REPLACE "\\" "\\\\" prefix ${prefix})
string(REPLACE "\\" "\\\\" SYSCONFDIR ${SYSCONFDIR})
string(REPLACE "\\" "\\\\" GR_PREFSDIR ${GR_PREFSDIR})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_constants.cc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gr_constants.cc
@ONLY)

list(APPEND gnuradio_core_sources ${CMAKE_CURRENT_BINARY_DIR}/gr_constants.cc)

########################################################################
# Append gnuradio-core library sources
########################################################################
list(APPEND gnuradio_core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_circular_file.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_count_bits.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fast_atan2f.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fft_vcc_fftw.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fxpt.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_misc.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_random.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_reverse.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_add_const_ss_generic.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_char_to_float.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_control_loop.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_debugger_hook.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_fft.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_float_to_char.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_float_to_int.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_float_to_short.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_float_to_uchar.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_glfsr.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_interleaved_short_to_complex.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_int_to_float.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_short_to_float.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_uchar_to_float.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/malloc16.c
)

########################################################################
# Append gnuradio-core test sources
########################################################################
list(APPEND test_gnuradio_core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_general.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_circular_file.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_cpm.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_firdes.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_fxpt.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_fxpt_nco.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_fxpt_vco.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_math.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gri_lfsr.cc
)

########################################################################
# Install runtime headers
########################################################################
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_core_api.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_circular_file.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_constants.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_count_bits.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_expj.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fft_vcc_fftw.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fxpt.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fxpt_nco.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fxpt_vco.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_log2_const.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_math.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_misc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_nco.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_random.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_reverse.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_simple_framer_sync.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_test_types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_vco.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_add_const_ss.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_agc_cc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_agc_ff.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_agc2_cc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_agc2_ff.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_char_to_float.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_control_loop.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_debugger_hook.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_fft.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_float_to_char.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_float_to_int.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_float_to_short.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_float_to_uchar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_lfsr.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_glfsr.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_interleaved_short_to_complex.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_lfsr_15_1_0.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_lfsr_32k.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_int_to_float.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_short_to_float.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_uchar_to_float.h
    ${CMAKE_CURRENT_SOURCE_DIR}/malloc16.h
    ${CMAKE_CURRENT_SOURCE_DIR}/random.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "core_devel"
)

########################################################################
# Install swig headers
########################################################################
if(ENABLE_PYTHON)
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/general.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_constants.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_agc_cc.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_agc_ff.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_agc2_cc.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_agc2_ff.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_control_loop.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "core_swig"
)
endif(ENABLE_PYTHON)

########################################################################
# Handle triple-threat files that have cc, h, and i
########################################################################
set(gr_core_general_triple_threats
    complex_vec_test
    gr_add_ff
    gr_additive_scrambler_bb
    gr_agc_cc
    gr_agc_ff
    gr_agc2_cc
    gr_agc2_ff
    gr_align_on_samplenumbers_ss
    gr_bin_statistics_f
    gr_bytes_to_syms
    gr_char_to_float
    gr_char_to_short
    gr_check_counting_s
    gr_check_lfsr_32k_s
    gr_complex_to_interleaved_short
    gr_complex_to_xxx
    gr_conjugate_cc
    gr_copy
    gr_cpfsk_bc
    gr_cpm
    gr_ctcss_squelch_ff
    gr_decode_ccsds_27_fb
    gr_diff_decoder_bb
    gr_diff_encoder_bb
    gr_diff_phasor_cc
    gr_dpll_bb
    gr_deinterleave
    gr_delay
    gr_encode_ccsds_27_bb
    gr_fake_channel_coder_pp
    gr_feedforward_agc_cc
    gr_feval
    gr_fft_vcc
    gr_fft_vfc
    gr_firdes
    gr_float_to_char
    gr_float_to_complex
    gr_float_to_int
    gr_float_to_short
    gr_float_to_uchar
    gr_fmdet_cf
    gr_frequency_modulator_fc
    gr_framer_sink_1
    gr_glfsr_source_b
    gr_glfsr_source_f
    gr_head
    gr_int_to_float
    gr_interleave
    gr_interleaved_short_to_complex
    gr_iqcomp_cc
    gr_keep_one_in_n
    gr_keep_m_in_n
    gr_kludge_copy
    gr_lfsr_32k_source_s
    gr_map_bb
    gr_multiply_cc
    gr_multiply_ff
    gr_multiply_const_cc
    gr_multiply_const_ff
    gr_multiply_conjugate_cc
    gr_nlog10_ff
    gr_nop
    gr_null_sink
    gr_null_source
    gr_pa_2x2_phase_combiner
    gr_packet_sink
    gr_peak_detector2_fb
    gr_phase_modulator_fc
    gr_pll_carriertracking_cc
    gr_pll_freqdet_cf
    gr_pll_refout_cc
    gr_pn_correlator_cc
    gr_prefs
    gr_probe_avg_mag_sqrd_c
    gr_probe_avg_mag_sqrd_cf
    gr_probe_avg_mag_sqrd_f
    gr_pwr_squelch_cc
    gr_pwr_squelch_ff
    gr_quadrature_demod_cf
    gr_rail_ff
    gr_regenerate_bb
    gr_remez
    gr_rms_cf
    gr_rms_ff
    gr_repeat
    gr_short_to_float
    gr_short_to_char
    gr_simple_correlator
    gr_simple_framer
    gr_simple_squelch_cc
    gr_skiphead
    gr_squelch_base_cc
    gr_squelch_base_ff
    gr_stream_mux
    gr_stream_to_streams
    gr_stream_to_vector
    gr_streams_to_stream
    gr_streams_to_vector
    gr_stretch_ff
    gr_test
    gr_threshold_ff
    gr_throttle
    gr_transcendental
    gr_uchar_to_float
    gr_vco_f
    gr_vector_map
    gr_vector_to_stream
    gr_vector_to_streams
    gr_unpack_k_bits_bb
    gr_pack_k_bits_bb
    gr_descrambler_bb
    gr_scrambler_bb
    gr_probe_density_b
    gr_annotator_alltoall
    gr_annotator_1to1
    gr_burst_tagger
    gr_correlate_access_code_tag_bb
)

foreach(file_tt ${gr_core_general_triple_threats})
    list(APPEND gnuradio_core_sources ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.cc)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.h DESTINATION ${GR_INCLUDE_DIR}/gnuradio COMPONENT "core_devel")
    if(ENABLE_PYTHON)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.i DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig COMPONENT "core_swig")
    endif(ENABLE_PYTHON)
endforeach(file_tt ${gr_core_general_triple_threats})
