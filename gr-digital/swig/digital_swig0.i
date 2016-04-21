/*
 * Copyright 2011-2015 Free Software Foundation, Inc.
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
#define ANALOG_API
#define BLOCKS_API

%include "gnuradio.i"
%include "stdint.i"

//load generated python docstrings
%include "digital_swig0_doc.i"

%include "gnuradio/analog/cpm.h"

%{
#include <gnuradio/blocks/control_loop.h>
%}
%include <gnuradio/blocks/control_loop.h>

%{
#include "gnuradio/digital/additive_scrambler_bb.h"
#include "gnuradio/digital/binary_slicer_fb.h"
#include "gnuradio/digital/burst_shaper_cc.h"
#include "gnuradio/digital/burst_shaper_ff.h"
#include "gnuradio/digital/chunks_to_symbols_bc.h"
#include "gnuradio/digital/chunks_to_symbols_bf.h"
#include "gnuradio/digital/chunks_to_symbols_ic.h"
#include "gnuradio/digital/chunks_to_symbols_if.h"
#include "gnuradio/digital/chunks_to_symbols_sc.h"
#include "gnuradio/digital/chunks_to_symbols_sf.h"
#include "gnuradio/digital/clock_recovery_mm_cc.h"
#include "gnuradio/digital/clock_recovery_mm_ff.h"
#include "gnuradio/digital/cma_equalizer_cc.h"
#include "gnuradio/digital/constellation.h"
#include "gnuradio/digital/constellation_decoder_cb.h"
#include "gnuradio/digital/constellation_receiver_cb.h"
#include "gnuradio/digital/constellation_soft_decoder_cf.h"
#include "gnuradio/digital/corr_est_cc.h"
#include "gnuradio/digital/correlate_access_code_bb.h"
#include "gnuradio/digital/correlate_access_code_tag_bb.h"
#include "gnuradio/digital/correlate_access_code_bb_ts.h"
#include "gnuradio/digital/correlate_access_code_ff_ts.h"
#include "gnuradio/digital/correlate_and_sync_cc.h"
#include "gnuradio/digital/costas_loop_cc.h"
#include "gnuradio/digital/cpmmod_bc.h"
#include "gnuradio/digital/crc32.h"
#include "gnuradio/digital/crc32_bb.h"
#include "gnuradio/digital/crc32_async_bb.h"
#include "gnuradio/digital/descrambler_bb.h"
#include "gnuradio/digital/diff_decoder_bb.h"
#include "gnuradio/digital/diff_encoder_bb.h"
#include "gnuradio/digital/diff_phasor_cc.h"
#include "gnuradio/digital/fll_band_edge_cc.h"
#include "gnuradio/digital/framer_sink_1.h"
#include "gnuradio/digital/glfsr_source_b.h"
#include "gnuradio/digital/glfsr_source_f.h"
#include "gnuradio/digital/hdlc_deframer_bp.h"
#include "gnuradio/digital/hdlc_framer_pb.h"
#include "gnuradio/digital/header_payload_demux.h"
#include "gnuradio/digital/kurtotic_equalizer_cc.h"
#include "gnuradio/digital/lfsr.h"
#include "gnuradio/digital/lms_dd_equalizer_cc.h"
#include "gnuradio/digital/map_bb.h"
#include "gnuradio/digital/metric_type.h"
#include "gnuradio/digital/modulate_vector.h"
#include "gnuradio/digital/mpsk_receiver_cc.h"
#include "gnuradio/digital/mpsk_snr_est.h"
#include "gnuradio/digital/mpsk_snr_est_cc.h"
%}

%include "gnuradio/digital/additive_scrambler_bb.h"
%include "gnuradio/digital/binary_slicer_fb.h"
%include "gnuradio/digital/burst_shaper_cc.h"
%include "gnuradio/digital/burst_shaper_ff.h"
%include "gnuradio/digital/chunks_to_symbols_bc.h"
%include "gnuradio/digital/chunks_to_symbols_bf.h"
%include "gnuradio/digital/chunks_to_symbols_ic.h"
%include "gnuradio/digital/chunks_to_symbols_if.h"
%include "gnuradio/digital/chunks_to_symbols_sc.h"
%include "gnuradio/digital/chunks_to_symbols_sf.h"
%include "gnuradio/digital/clock_recovery_mm_cc.h"
%include "gnuradio/digital/clock_recovery_mm_ff.h"
%include "gnuradio/digital/cma_equalizer_cc.h"
%include "gnuradio/digital/constellation.h"
%include "gnuradio/digital/constellation_decoder_cb.h"
%include "gnuradio/digital/constellation_receiver_cb.h"
%include "gnuradio/digital/constellation_soft_decoder_cf.h"
%include "gnuradio/digital/corr_est_cc.h"
%include "gnuradio/digital/correlate_access_code_bb.h"
%include "gnuradio/digital/correlate_access_code_tag_bb.h"
%include "gnuradio/digital/correlate_access_code_bb_ts.h"
%include "gnuradio/digital/correlate_access_code_ff_ts.h"
%include "gnuradio/digital/correlate_and_sync_cc.h"
%include "gnuradio/digital/costas_loop_cc.h"
%include "gnuradio/digital/cpmmod_bc.h"
%include "gnuradio/digital/crc32.h"
%include "gnuradio/digital/crc32_bb.h"
%include "gnuradio/digital/crc32_async_bb.h"
%include "gnuradio/digital/descrambler_bb.h"
%include "gnuradio/digital/diff_decoder_bb.h"
%include "gnuradio/digital/diff_encoder_bb.h"
%include "gnuradio/digital/diff_phasor_cc.h"
%include "gnuradio/digital/fll_band_edge_cc.h"
%include "gnuradio/digital/framer_sink_1.h"
%include "gnuradio/digital/glfsr_source_b.h"
%include "gnuradio/digital/glfsr_source_f.h"
%include "gnuradio/digital/hdlc_deframer_bp.h"
%include "gnuradio/digital/hdlc_framer_pb.h"
%include "gnuradio/digital/header_payload_demux.h"
%include "gnuradio/digital/kurtotic_equalizer_cc.h"
%include "gnuradio/digital/lfsr.h"
%include "gnuradio/digital/lms_dd_equalizer_cc.h"
%include "gnuradio/digital/map_bb.h"
%include "gnuradio/digital/metric_type.h"
%include "gnuradio/digital/modulate_vector.h"
%include "gnuradio/digital/mpsk_receiver_cc.h"
%include "gnuradio/digital/mpsk_snr_est.h"
%include "gnuradio/digital/mpsk_snr_est_cc.h"

GR_SWIG_BLOCK_MAGIC2(digital, additive_scrambler_bb);
GR_SWIG_BLOCK_MAGIC2(digital, binary_slicer_fb);
GR_SWIG_BLOCK_MAGIC2(digital, burst_shaper_cc);
GR_SWIG_BLOCK_MAGIC2(digital, burst_shaper_ff);
GR_SWIG_BLOCK_MAGIC2(digital, chunks_to_symbols_bc);
GR_SWIG_BLOCK_MAGIC2(digital, chunks_to_symbols_bf);
GR_SWIG_BLOCK_MAGIC2(digital, chunks_to_symbols_ic);
GR_SWIG_BLOCK_MAGIC2(digital, chunks_to_symbols_if);
GR_SWIG_BLOCK_MAGIC2(digital, chunks_to_symbols_sc);
GR_SWIG_BLOCK_MAGIC2(digital, chunks_to_symbols_sf);
GR_SWIG_BLOCK_MAGIC2(digital, clock_recovery_mm_cc);
GR_SWIG_BLOCK_MAGIC2(digital, clock_recovery_mm_ff);
GR_SWIG_BLOCK_MAGIC2(digital, cma_equalizer_cc);
GR_SWIG_BLOCK_MAGIC2(digital, constellation_decoder_cb);
GR_SWIG_BLOCK_MAGIC2(digital, constellation_receiver_cb);
GR_SWIG_BLOCK_MAGIC2(digital, constellation_soft_decoder_cf);
GR_SWIG_BLOCK_MAGIC2(digital, corr_est_cc);
GR_SWIG_BLOCK_MAGIC2(digital, correlate_access_code_bb);
GR_SWIG_BLOCK_MAGIC2(digital, correlate_access_code_tag_bb);
GR_SWIG_BLOCK_MAGIC2(digital, correlate_access_code_bb_ts);
GR_SWIG_BLOCK_MAGIC2(digital, correlate_access_code_ff_ts);
GR_SWIG_BLOCK_MAGIC2(digital, correlate_and_sync_cc);
GR_SWIG_BLOCK_MAGIC2(digital, costas_loop_cc);
GR_SWIG_BLOCK_MAGIC2(digital, crc32_bb);
GR_SWIG_BLOCK_MAGIC2(digital, crc32_async_bb);
GR_SWIG_BLOCK_MAGIC2(digital, cpmmod_bc);
GR_SWIG_BLOCK_MAGIC2(digital, descrambler_bb);
GR_SWIG_BLOCK_MAGIC2(digital, diff_decoder_bb);
GR_SWIG_BLOCK_MAGIC2(digital, diff_encoder_bb);
GR_SWIG_BLOCK_MAGIC2(digital, diff_phasor_cc);
GR_SWIG_BLOCK_MAGIC2(digital, fll_band_edge_cc);
GR_SWIG_BLOCK_MAGIC2(digital, framer_sink_1);
GR_SWIG_BLOCK_MAGIC2(digital, glfsr_source_b);
GR_SWIG_BLOCK_MAGIC2(digital, glfsr_source_f);
GR_SWIG_BLOCK_MAGIC2(digital, hdlc_deframer_bp);
GR_SWIG_BLOCK_MAGIC2(digital, hdlc_framer_pb);
GR_SWIG_BLOCK_MAGIC2(digital, header_payload_demux);
GR_SWIG_BLOCK_MAGIC2(digital, kurtotic_equalizer_cc);
GR_SWIG_BLOCK_MAGIC2(digital, lms_dd_equalizer_cc);
GR_SWIG_BLOCK_MAGIC2(digital, map_bb);
GR_SWIG_BLOCK_MAGIC2(digital, mpsk_receiver_cc);
GR_SWIG_BLOCK_MAGIC2(digital, mpsk_snr_est_cc);

GR_SWIG_BLOCK_MAGIC_FACTORY(digital, cpmmod_bc, gmskmod_bc);

%include "constellation.i"
