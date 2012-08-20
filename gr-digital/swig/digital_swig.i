/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#define DIGITAL_API

%include "gnuradio.i"

//load generated python docstrings
%include "digital_swig_doc.i"

%include "gr_cpm.h"

%include "gri_control_loop.h"

// Used in the constellation objects
%template(unsigned_int_vector) std::vector<unsigned int>;

%{
#include "digital/metric_type.h"
#include "digital/mpsk_snr_est.h"
#include "digital/additive_scrambler_bb.h"
#include "digital/binary_slicer_fb.h"
#include "digital/chunks_to_symbols_bf.h"
#include "digital/chunks_to_symbols_bc.h"
#include "digital/chunks_to_symbols_sf.h"
#include "digital/chunks_to_symbols_sc.h"
#include "digital/chunks_to_symbols_if.h"
#include "digital/chunks_to_symbols_ic.h"
#include "digital/clock_recovery_mm_cc.h"
#include "digital/clock_recovery_mm_ff.h"
#include "digital/cma_equalizer_cc.h"
#include "digital/constellation.h"
#include "digital/constellation_receiver_cb.h"
#include "digital/constellation_decoder_cb.h"
#include "digital/correlate_access_code_bb.h"
#include "digital/correlate_access_code_tag_bb.h"
#include "digital/costas_loop_cc.h"
#include "digital/cpmmod_bc.h"
#include "digital/crc32.h"
#include "digital/descrambler_bb.h"
#include "digital/diff_decoder_bb.h"
#include "digital/diff_encoder_bb.h"
#include "digital/diff_phasor_cc.h"
#include "digital/fll_band_edge_cc.h"
#include "digital/framer_sink_1.h"
#include "digital/glfsr_source_b.h"
#include "digital/glfsr_source_f.h"
#include "digital/kurtotic_equalizer_cc.h"
#include "digital/lms_dd_equalizer_cc.h"
#include "digital/map_bb.h"
#include "digital/mpsk_receiver_cc.h"
#include "digital/mpsk_snr_est_cc.h"
#include "digital/ofdm_cyclic_prefixer.h"
#include "digital/ofdm_frame_acquisition.h"
#include "digital/ofdm_frame_sink.h"
#include "digital/ofdm_insert_preamble.h"
#include "digital/ofdm_mapper_bcv.h"
#include "digital/ofdm_sampler.h"
#include "digital/packet_sink.h"
#include "digital/pfb_clock_sync_ccf.h"
#include "digital/pfb_clock_sync_fff.h"
#include "digital/pn_correlator_cc.h"
#include "digital/probe_density_b.h"
#include "digital/probe_mpsk_snr_est_c.h"
#include "digital/scrambler_bb.h"
#include "digital/simple_framer.h"
%}

%include "digital/metric_type.h"
%include "digital/mpsk_snr_est.h"
%include "digital/additive_scrambler_bb.h"
%include "digital/binary_slicer_fb.h"
%include "digital/chunks_to_symbols_bf.h"
%include "digital/chunks_to_symbols_bc.h"
%include "digital/chunks_to_symbols_sf.h"
%include "digital/chunks_to_symbols_sc.h"
%include "digital/chunks_to_symbols_if.h"
%include "digital/chunks_to_symbols_ic.h"
%include "digital/clock_recovery_mm_cc.h"
%include "digital/clock_recovery_mm_ff.h"
%include "digital/cma_equalizer_cc.h"
%include "digital/constellation.h"
%include "digital/constellation_receiver_cb.h"
%include "digital/constellation_decoder_cb.h"
%include "digital/correlate_access_code_bb.h"
%include "digital/correlate_access_code_tag_bb.h"
%include "digital/costas_loop_cc.h"
%include "digital/cpmmod_bc.h"
%include "digital/crc32.h"
%include "digital/descrambler_bb.h"
%include "digital/diff_decoder_bb.h"
%include "digital/diff_encoder_bb.h"
%include "digital/diff_phasor_cc.h"
%include "digital/fll_band_edge_cc.h"
%include "digital/framer_sink_1.h"
%include "digital/glfsr_source_b.h"
%include "digital/glfsr_source_f.h"
%include "digital/kurtotic_equalizer_cc.h"
%include "digital/lms_dd_equalizer_cc.h"
%include "digital/map_bb.h"
%include "digital/mpsk_receiver_cc.h"
%include "digital/mpsk_snr_est_cc.h"
%include "digital/ofdm_cyclic_prefixer.h"
%include "digital/ofdm_frame_acquisition.h"
%include "digital/ofdm_frame_sink.h"
%include "digital/ofdm_insert_preamble.h"
%include "digital/ofdm_mapper_bcv.h"
%include "digital/ofdm_sampler.h"
%include "digital/packet_sink.h"
%include "digital/pfb_clock_sync_ccf.h"
%include "digital/pfb_clock_sync_fff.h"
%include "digital/pn_correlator_cc.h"
%include "digital/probe_density_b.h"
%include "digital/probe_mpsk_snr_est_c.h"
%include "digital/scrambler_bb.h"
%include "digital/simple_framer.h"

GR_SWIG_BLOCK_MAGIC2(digital, additive_scrambler_bb);
GR_SWIG_BLOCK_MAGIC2(digital, binary_slicer_fb);
GR_SWIG_BLOCK_MAGIC2(digital, chunks_to_symbols_bf);
GR_SWIG_BLOCK_MAGIC2(digital, chunks_to_symbols_bc);
GR_SWIG_BLOCK_MAGIC2(digital, chunks_to_symbols_sf);
GR_SWIG_BLOCK_MAGIC2(digital, chunks_to_symbols_sc);
GR_SWIG_BLOCK_MAGIC2(digital, chunks_to_symbols_if);
GR_SWIG_BLOCK_MAGIC2(digital, chunks_to_symbols_ic);
GR_SWIG_BLOCK_MAGIC2(digital, clock_recovery_mm_cc);
GR_SWIG_BLOCK_MAGIC2(digital, clock_recovery_mm_ff);
GR_SWIG_BLOCK_MAGIC2(digital, cma_equalizer_cc);
GR_SWIG_BLOCK_MAGIC2(digital, constellation_receiver_cb);
GR_SWIG_BLOCK_MAGIC2(digital, constellation_decoder_cb);
GR_SWIG_BLOCK_MAGIC2(digital, correlate_access_code_bb);
GR_SWIG_BLOCK_MAGIC2(digital, correlate_access_code_tag_bb);
GR_SWIG_BLOCK_MAGIC2(digital, costas_loop_cc);
GR_SWIG_BLOCK_MAGIC2(digital, cpmmod_bc);
GR_SWIG_BLOCK_MAGIC2(digital, descrambler_bb);
GR_SWIG_BLOCK_MAGIC2(digital, diff_decoder_bb);
GR_SWIG_BLOCK_MAGIC2(digital, diff_encoder_bb);
GR_SWIG_BLOCK_MAGIC2(digital, diff_phasor_cc);
GR_SWIG_BLOCK_MAGIC2(digital, fll_band_edge_cc);
GR_SWIG_BLOCK_MAGIC2(digital, framer_sink_1);
GR_SWIG_BLOCK_MAGIC2(digital, glfsr_source_b);
GR_SWIG_BLOCK_MAGIC2(digital, glfsr_source_f);
GR_SWIG_BLOCK_MAGIC2(digital, kurtotic_equalizer_cc);
GR_SWIG_BLOCK_MAGIC2(digital, lms_dd_equalizer_cc);
GR_SWIG_BLOCK_MAGIC2(digital, map_bb);
GR_SWIG_BLOCK_MAGIC2(digital, mpsk_receiver_cc);
GR_SWIG_BLOCK_MAGIC2(digital, mpsk_snr_est_cc);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_cyclic_prefixer);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_frame_acquisition);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_frame_sink);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_insert_preamble);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_mapper_bcv);
GR_SWIG_BLOCK_MAGIC2(digital, ofdm_sampler);
GR_SWIG_BLOCK_MAGIC2(digital, packet_sink);
GR_SWIG_BLOCK_MAGIC2(digital, pfb_clock_sync_ccf);
GR_SWIG_BLOCK_MAGIC2(digital, pfb_clock_sync_fff);
GR_SWIG_BLOCK_MAGIC2(digital, pn_correlator_cc);
GR_SWIG_BLOCK_MAGIC2(digital, probe_density_b);
GR_SWIG_BLOCK_MAGIC2(digital, probe_mpsk_snr_est_c);
GR_SWIG_BLOCK_MAGIC2(digital, scrambler_bb);
GR_SWIG_BLOCK_MAGIC2(digital, simple_framer);

GR_SWIG_BLOCK_MAGIC_FACTORY(digital, cpmmod_bc, gmskmod_bc);

// Properly package up constellation objects
%include "constellation.i"
