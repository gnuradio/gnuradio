/* -*- c++ -*- */
/*
 * Copyright 2004,2005,2006,2007,2008,2009,2010 Free Software Foundation, Inc.
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

#include <gri_control_loop.h>
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
#include <gr_float_to_int.h>
#include <gr_float_to_short.h>
#include <gr_float_to_char.h>
#include <gr_float_to_uchar.h>
#include <gr_short_to_float.h>
#include <gr_short_to_char.h>
#include <gr_int_to_float.h>
#include <gr_char_to_float.h>
#include <gr_char_to_short.h>
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
//#include <gr_endianness.h>
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
#include <gr_transcendental.h>
#include <gr_stream_mux.h>
#include <gr_stream_to_streams.h>
#include <gr_streams_to_stream.h>
#include <gr_streams_to_vector.h>
#include <gr_vector_to_streams.h>
#include <gr_conjugate_cc.h>
#include <gr_vco_f.h>
#include <gr_threshold_ff.h>
#include <gr_packet_sink.h>
#include <gr_dpll_bb.h>
#include <gr_fmdet_cf.h>
#include <gr_pll_freqdet_cf.h>
#include <gr_pll_refout_cc.h>
#include <gr_pll_carriertracking_cc.h>
#include <gr_pn_correlator_cc.h>
#include <gr_probe_avg_mag_sqrd_c.h>
#include <gr_probe_avg_mag_sqrd_cf.h>
#include <gr_probe_avg_mag_sqrd_f.h>
#include <gr_regenerate_bb.h>
#include <gr_pa_2x2_phase_combiner.h>
#include <gr_kludge_copy.h>
#include <gr_prefs.h>
#include <gr_constants.h>
#include <gr_test_types.h>
#include <gr_test.h>
#include <gr_unpack_k_bits_bb.h>
#include <gr_diff_phasor_cc.h>
#include <gr_diff_encoder_bb.h>
#include <gr_diff_decoder_bb.h>
#include <gr_framer_sink_1.h>
#include <gr_map_bb.h>
#include <gr_multiply_cc.h>
#include <gr_multiply_ff.h>
#include <gr_multiply_const_cc.h>
#include <gr_multiply_const_ff.h>
#include <gr_multiply_conjugate_cc.h>
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
#include <gr_cpfsk_bc.h>
#include <gr_encode_ccsds_27_bb.h>
#include <gr_decode_ccsds_27_fb.h>
#include <gr_descrambler_bb.h>
#include <gr_scrambler_bb.h>
#include <gr_probe_mpsk_snr_c.h>
#include <gr_probe_density_b.h>
#include <gr_rail_ff.h>
#include <gr_squash_ff.h>
#include <gr_stretch_ff.h>
#include <gr_wavelet_ff.h>
#include <gr_wvps_ff.h>
#include <gr_copy.h>
#include <gr_additive_scrambler_bb.h>
#include <complex_vec_test.h>
#include <gr_annotator_alltoall.h>
#include <gr_annotator_1to1.h>
#include <gr_burst_tagger.h>
#include <gr_cpm.h>
#include <gr_correlate_access_code_tag_bb.h>
#include <gr_add_ff.h>
%}

%include "gri_control_loop.i"
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
%include "gr_float_to_int.i"
%include "gr_float_to_short.i"
%include "gr_float_to_char.i"
%include "gr_float_to_uchar.i"
%include "gr_short_to_float.i"
%include "gr_short_to_char.i"
%include "gr_int_to_float.i"
%include "gr_char_to_float.i"
%include "gr_char_to_short.i"
%include "gr_uchar_to_float.i"
%include "gr_frequency_modulator_fc.i"
%include "gr_phase_modulator_fc.i"
%include "gr_bytes_to_syms.i"
%include "gr_simple_correlator.i"
%include "gr_simple_framer.i"
%include "gr_align_on_samplenumbers_ss.i"
%include "gr_complex_to_xxx.i"
%include "gr_complex_to_interleaved_short.i"
//%include "gr_endianness.i"
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
%include "gr_transcendental.i"
%include "gr_stream_mux.i"
%include "gr_stream_to_streams.i"
%include "gr_streams_to_stream.i"
%include "gr_streams_to_vector.i"
%include "gr_vector_to_streams.i"
%include "gr_conjugate_cc.i"
%include "gr_vco_f.i"
%include "gr_threshold_ff.i"
%include "gr_packet_sink.i"
%include "gr_dpll_bb.i"
%include "gr_fmdet_cf.i"
%include "gr_pll_freqdet_cf.i"
%include "gr_pll_refout_cc.i"
%include "gr_pll_carriertracking_cc.i"
%include "gr_pn_correlator_cc.i"
%include "gr_probe_avg_mag_sqrd_c.i"
%include "gr_probe_avg_mag_sqrd_cf.i"
%include "gr_probe_avg_mag_sqrd_f.i"
%include "gr_regenerate_bb.i"
%include "gr_pa_2x2_phase_combiner.i"
%include "gr_kludge_copy.i"
%include "gr_prefs.i"
%include "gr_constants.i"
%include "gr_test_types.h"
%include "gr_test.i"
%include "gr_unpack_k_bits_bb.i"
%include "gr_diff_phasor_cc.i"
%include "gr_diff_encoder_bb.i"
%include "gr_diff_decoder_bb.i"
%include "gr_framer_sink_1.i"
%include "gr_map_bb.i"
%include "gr_multiply_cc.i"
%include "gr_multiply_ff.i"
%include "gr_multiply_const_cc.i"
%include "gr_multiply_const_ff.i"
%include "gr_multiply_conjugate_cc.i"
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
%include "gr_cpfsk_bc.i"
%include "gr_encode_ccsds_27_bb.i"
%include "gr_decode_ccsds_27_fb.i"
%include "gr_descrambler_bb.i"
%include "gr_scrambler_bb.i"
%include "gr_probe_mpsk_snr_c.i"
%include "gr_probe_density_b.i"
%include "gr_rail_ff.i"
%include "gr_squash_ff.i"
%include "gr_stretch_ff.i"
%include "gr_wavelet_ff.i"
%include "gr_wvps_ff.i"
%include "gr_copy.i"
%include "gr_additive_scrambler_bb.i"
%include "complex_vec_test.i"
%include "gr_annotator_alltoall.i"
%include "gr_annotator_1to1.i"
%include "gr_burst_tagger.i"
%include "gr_cpm.i"
%include "gr_correlate_access_code_tag_bb.i"
%include "gr_add_ff.i"
