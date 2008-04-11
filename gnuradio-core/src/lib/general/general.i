/* -*- c++ -*- */
/*
 * Copyright 2004,2005,2006,2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

%{

#include <gr_nop.h>
#include <gr_null_sink.h>
#include <gr_null_source.h>
#include <gr_head.h>
#include <gr_skiphead.h>
#include <gr_quadrature_demod_cf.h>
#include <gr_remez.h>
#include <gr_float_to_complex.h>
#include <gr_check_counting_s.h>
#include <gr_lfsr_32k_source_s.h>
#include <gr_check_lfsr_32k_s.h>
#include <gr_stream_to_vector.h>
#include <gr_vector_to_stream.h>
#include <gr_keep_one_in_n.h>
#include <gr_fft_vcc.h>
#include <gr_fft_vfc.h>
#include <gr_float_to_short.h>
#include <gr_float_to_char.h>
#include <gr_float_to_uchar.h>
#include <gr_short_to_float.h>
#include <gr_char_to_float.h>
#include <gr_uchar_to_float.h>
#include <gr_frequency_modulator_fc.h>
#include <gr_phase_modulator_fc.h>
#include <gr_bytes_to_syms.h>
#include <gr_simple_correlator.h>
#include <gr_simple_framer.h>
#include <gr_align_on_samplenumbers_ss.h>
#include <gr_complex_to_xxx.h>
#include <gr_complex_to_interleaved_short.h>
#include <gr_interleaved_short_to_complex.h>
#include <gr_endianness.h>
#include <gr_firdes.h>
#include <gr_interleave.h>
#include <gr_deinterleave.h>
#include <gr_delay.h>
#include <gr_simple_squelch_cc.h>
#include <gr_agc_ff.h>
#include <gr_agc_cc.h>
#include <gr_agc2_ff.h>
#include <gr_agc2_cc.h>
#include <gr_rms_cf.h>
#include <gr_rms_ff.h>
#include <gr_nlog10_ff.h>
#include <gr_fake_channel_coder_pp.h>
#include <gr_throttle.h>
#include <gr_mpsk_receiver_cc.h>
#include <gr_stream_mux.h>
#include <gr_stream_to_streams.h>
#include <gr_streams_to_stream.h>
#include <gr_streams_to_vector.h>
#include <gr_vector_to_streams.h>
#include <gr_conjugate_cc.h>
#include <gr_vco_f.h>
#include <gr_crc32.h>
#include <gr_threshold_ff.h>
#include <gr_clock_recovery_mm_ff.h>
#include <gr_clock_recovery_mm_cc.h>
#include <gr_dd_mpsk_sync_cc.h>
#include <gr_packet_sink.h>
#include <gr_lms_dfe_cc.h>
#include <gr_lms_dfe_ff.h>
#include <gr_dpll_bb.h>
#include <gr_pll_freqdet_cf.h>
#include <gr_pll_refout_cc.h>
#include <gr_pll_carriertracking_cc.h>
#include <gr_pn_correlator_cc.h>
#include <gr_probe_avg_mag_sqrd_c.h>
#include <gr_probe_avg_mag_sqrd_cf.h>
#include <gr_probe_avg_mag_sqrd_f.h>
#include <gr_probe_signal_f.h>
#include <gr_ofdm_frame_acquisition.h>
#include <gr_ofdm_cyclic_prefixer.h>
#include <gr_ofdm_bpsk_demapper.h>
#include <gr_ofdm_mapper_bcv.h>
#include <gr_ofdm_bpsk_mapper.h>
#include <gr_ofdm_qpsk_mapper.h>
#include <gr_ofdm_qam_mapper.h>
#include <gr_ofdm_frame_sink.h>
#include <gr_ofdm_insert_preamble.h>
#include <gr_ofdm_sampler.h>
#include <gr_regenerate_bb.h>
#include <gr_costas_loop_cc.h>
#include <gr_pa_2x2_phase_combiner.h>
#include <gr_kludge_copy.h>
#include <gr_prefs.h>
#include <gr_prefix.h>
#include <gr_test_types.h>
#include <gr_test.h>
#include <gr_unpack_k_bits_bb.h>
#include <gr_correlate_access_code_bb.h>
#include <gr_diff_phasor_cc.h>
#include <gr_constellation_decoder_cb.h>
#include <gr_binary_slicer_fb.h>
#include <gr_diff_encoder_bb.h>
#include <gr_diff_decoder_bb.h>
#include <gr_framer_sink_1.h>
#include <gr_map_bb.h>
#include <gr_feval.h>
#include <gr_pwr_squelch_cc.h>
#include <gr_pwr_squelch_ff.h>
#include <gr_ctcss_squelch_ff.h>
#include <gr_feedforward_agc_cc.h>
#include <gr_bin_statistics_f.h>
#include <gr_glfsr_source_b.h>
#include <gr_glfsr_source_f.h>
#include <gr_peak_detector2_fb.h>
#include <gr_repeat.h>
%}

%include "gr_nop.i"
%include "gr_null_sink.i"
%include "gr_null_source.i"
%include "gr_head.i"
%include "gr_skiphead.i"
%include "gr_quadrature_demod_cf.i"
%include "gr_remez.i"
%include "gr_float_to_complex.i"
%include "gr_check_counting_s.i"
%include "gr_lfsr_32k_source_s.i"
%include "gr_check_lfsr_32k_s.i"
%include "gr_stream_to_vector.i"
%include "gr_vector_to_stream.i"
%include "gr_keep_one_in_n.i"
%include "gr_fft_vcc.i"
%include "gr_fft_vfc.i"
%include "gr_float_to_short.i"
%include "gr_float_to_char.i"
%include "gr_float_to_uchar.i"
%include "gr_short_to_float.i"
%include "gr_char_to_float.i"
%include "gr_uchar_to_float.i"
%include "gr_frequency_modulator_fc.i"
%include "gr_phase_modulator_fc.i"
%include "gr_bytes_to_syms.i"
%include "gr_simple_correlator.i"
%include "gr_simple_framer.i"
%include "gr_align_on_samplenumbers_ss.i"
%include "gr_complex_to_xxx.i"
%include "gr_complex_to_interleaved_short.i"
%include "gr_endianness.i"
%include "gr_interleaved_short_to_complex.i"
%include "gr_firdes.i"
%include "gr_interleave.i"
%include "gr_deinterleave.i"
%include "gr_delay.i"
%include "gr_simple_squelch_cc.i"
%include "gr_agc_ff.i"
%include "gr_agc_cc.i"
%include "gr_agc2_ff.i"
%include "gr_agc2_cc.i"
%include "gr_rms_cf.i"
%include "gr_rms_ff.i"
%include "gr_nlog10_ff.i"
%include "gr_fake_channel_coder_pp.i"
%include "gr_throttle.i"
%include "gr_mpsk_receiver_cc.i"
%include "gr_stream_mux.i"
%include "gr_stream_to_streams.i"
%include "gr_streams_to_stream.i"
%include "gr_streams_to_vector.i"
%include "gr_vector_to_streams.i"
%include "gr_conjugate_cc.i"
%include "gr_vco_f.i"
%include "gr_crc32.i"
%include "gr_threshold_ff.i"
%include "gr_clock_recovery_mm_ff.i"
%include "gr_clock_recovery_mm_cc.i"
%include "gr_dd_mpsk_sync_cc.i"
%include "gr_packet_sink.i"
%include "gr_lms_dfe_cc.i"
%include "gr_lms_dfe_ff.i"
%include "gr_dpll_bb.i"
%include "gr_pll_freqdet_cf.i"
%include "gr_pll_refout_cc.i"
%include "gr_pll_carriertracking_cc.i"
%include "gr_pn_correlator_cc.i"
%include "gr_probe_avg_mag_sqrd_c.i"
%include "gr_probe_avg_mag_sqrd_cf.i"
%include "gr_probe_avg_mag_sqrd_f.i"
%include "gr_probe_signal_f.i"
%include "gr_ofdm_frame_acquisition.i"
%include "gr_ofdm_cyclic_prefixer.i"
%include "gr_ofdm_bpsk_demapper.i"
%include "gr_ofdm_mapper_bcv.i"
%include "gr_ofdm_bpsk_mapper.i"
%include "gr_ofdm_qpsk_mapper.i"
%include "gr_ofdm_qam_mapper.i"
%include "gr_ofdm_frame_sink.i"
%include "gr_ofdm_insert_preamble.i"
%include "gr_ofdm_sampler.i"
%include "gr_regenerate_bb.i"
%include "gr_costas_loop_cc.i"
%include "gr_pa_2x2_phase_combiner.i"
%include "gr_kludge_copy.i"
%include "gr_prefs.i"
%include "gr_prefix.i"
%include "gr_test_types.h"
%include "gr_test.i"
%include "gr_unpack_k_bits_bb.i"
%include "gr_correlate_access_code_bb.i"
%include "gr_diff_phasor_cc.i"
%include "gr_constellation_decoder_cb.i"
%include "gr_binary_slicer_fb.i"
%include "gr_diff_encoder_bb.i"
%include "gr_diff_decoder_bb.i"
%include "gr_framer_sink_1.i"
%include "gr_map_bb.i"
%include "gr_feval.i"
%include "gr_pwr_squelch_cc.i"
%include "gr_pwr_squelch_ff.i"
%include "gr_ctcss_squelch_ff.i"
%include "gr_feedforward_agc_cc.i"
%include "gr_bin_statistics_f.i"
%include "gr_glfsr_source_b.i"
%include "gr_glfsr_source_f.i"
%include "gr_peak_detector2_fb.i"
%include "gr_repeat.i"
