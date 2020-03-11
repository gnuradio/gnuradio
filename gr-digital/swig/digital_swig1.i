/*
 * Copyright 2016,2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#define DIGITAL_API
#define ANALOG_API
#define BLOCKS_API

%include "gnuradio.i"
%include "stdint.i"

//load generated python docstrings
%include "digital_swig1_doc.i"

 //%include "gnuradio/analog/cpm.h"

%{
#include <gnuradio/blocks/control_loop.h>
%}
%include <gnuradio/blocks/control_loop.h>

%{
#include "gnuradio/digital/additive_scrambler_bb.h"
#include "gnuradio/digital/burst_shaper.h"
#include "gnuradio/digital/chunks_to_symbols.h"
#include "gnuradio/digital/clock_recovery_mm_cc.h"
#include "gnuradio/digital/clock_recovery_mm_ff.h"
#include "gnuradio/digital/diff_decoder_bb.h"
#include "gnuradio/digital/diff_encoder_bb.h"
#include "gnuradio/digital/diff_phasor_cc.h"
#include "gnuradio/digital/fll_band_edge_cc.h"
#include "gnuradio/digital/framer_sink_1.h"
#include "gnuradio/digital/glfsr.h"
#include "gnuradio/digital/glfsr_source_b.h"
#include "gnuradio/digital/glfsr_source_f.h"
#include "gnuradio/digital/hdlc_deframer_bp.h"
#include "gnuradio/digital/hdlc_framer_pb.h"
#include "gnuradio/digital/header_payload_demux.h"
#include "gnuradio/digital/kurtotic_equalizer_cc.h"
#include "gnuradio/digital/lfsr.h"
#include "gnuradio/digital/map_bb.h"
#include "gnuradio/digital/metric_type.h"
#include "gnuradio/digital/modulate_vector.h"
#include "gnuradio/digital/msk_timing_recovery_cc.h"
%}

%include "gnuradio/digital/additive_scrambler_bb.h"
%include "gnuradio/digital/burst_shaper.h"
%include "gnuradio/digital/chunks_to_symbols.h"
%include "gnuradio/digital/clock_recovery_mm_cc.h"
%include "gnuradio/digital/clock_recovery_mm_ff.h"
%include "gnuradio/digital/diff_decoder_bb.h"
%include "gnuradio/digital/diff_encoder_bb.h"
%include "gnuradio/digital/diff_phasor_cc.h"
%include "gnuradio/digital/fll_band_edge_cc.h"
%include "gnuradio/digital/framer_sink_1.h"
%include "gnuradio/digital/glfsr.h"
%include "gnuradio/digital/glfsr_source_b.h"
%include "gnuradio/digital/glfsr_source_f.h"
%include "gnuradio/digital/hdlc_deframer_bp.h"
%include "gnuradio/digital/hdlc_framer_pb.h"
%include "gnuradio/digital/header_payload_demux.h"
%include "gnuradio/digital/kurtotic_equalizer_cc.h"
%include "gnuradio/digital/lfsr.h"
%include "gnuradio/digital/map_bb.h"
%include "gnuradio/digital/metric_type.h"
%include "gnuradio/digital/modulate_vector.h"
%include "gnuradio/digital/msk_timing_recovery_cc.h"

GR_SWIG_BLOCK_MAGIC2(digital, additive_scrambler_bb);
GR_SWIG_BLOCK_MAGIC2_TMPL(digital, burst_shaper_cc, burst_shaper<gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(digital, burst_shaper_ff, burst_shaper<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(digital, chunks_to_symbols_bc, chunks_to_symbols<std::uint8_t,gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(digital, chunks_to_symbols_bf, chunks_to_symbols<std::uint8_t,float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(digital, chunks_to_symbols_ic, chunks_to_symbols<std::int32_t,gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(digital, chunks_to_symbols_if, chunks_to_symbols<std::int32_t,float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(digital, chunks_to_symbols_sc, chunks_to_symbols<std::int16_t,gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(digital, chunks_to_symbols_sf, chunks_to_symbols<std::int16_t,float>);
GR_SWIG_BLOCK_MAGIC2(digital, clock_recovery_mm_cc);
GR_SWIG_BLOCK_MAGIC2(digital, clock_recovery_mm_ff);
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
GR_SWIG_BLOCK_MAGIC2(digital, map_bb);
GR_SWIG_BLOCK_MAGIC2(digital, msk_timing_recovery_cc);
