/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
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

#define BLOCKS_API

%include "gnuradio.i"
%include "blocks/pdu.h"

%include <gr_endianness.h>

//load generated python docstrings
%include "blocks_swig1_doc.i"

%{
#include "blocks/nlog10_ff.h"
#include "blocks/not_bb.h"
#include "blocks/not_ss.h"
#include "blocks/not_ii.h"
#include "blocks/patterned_interleaver.h"
#include "blocks/pack_k_bits_bb.h"
#include "blocks/packed_to_unpacked_bb.h"
#include "blocks/packed_to_unpacked_ss.h"
#include "blocks/packed_to_unpacked_ii.h"
#include "blocks/pdu_to_tagged_stream.h"
#include "blocks/peak_detector_fb.h"
#include "blocks/peak_detector_ib.h"
#include "blocks/peak_detector_sb.h"
#include "blocks/peak_detector2_fb.h"
#include "blocks/plateau_detector_fb.h"
#include "blocks/probe_rate.h"
#include "blocks/probe_signal_b.h"
#include "blocks/probe_signal_s.h"
#include "blocks/probe_signal_i.h"
#include "blocks/probe_signal_f.h"
#include "blocks/probe_signal_c.h"
#include "blocks/probe_signal_vb.h"
#include "blocks/probe_signal_vs.h"
#include "blocks/probe_signal_vi.h"
#include "blocks/probe_signal_vf.h"
#include "blocks/probe_signal_vc.h"
#include "blocks/or_bb.h"
#include "blocks/or_ss.h"
#include "blocks/or_ii.h"
#include "blocks/regenerate_bb.h"
#include "blocks/repack_bits_bb.h"
#include "blocks/repeat.h"
#include "blocks/rms_cf.h"
#include "blocks/rms_ff.h"
#include "blocks/sample_and_hold_bb.h"
#include "blocks/sample_and_hold_ss.h"
#include "blocks/sample_and_hold_ii.h"
#include "blocks/sample_and_hold_ff.h"
#include "blocks/short_to_char.h"
#include "blocks/short_to_float.h"
#include "blocks/socket_pdu.h"
#include "blocks/stretch_ff.h"
#include "blocks/sub_ff.h"
#include "blocks/sub_ss.h"
#include "blocks/sub_ii.h"
#include "blocks/sub_cc.h"
#include "blocks/tagged_stream_mux.h"
#include "blocks/tagged_stream_to_pdu.h"
#include "blocks/threshold_ff.h"
#include "blocks/transcendental.h"
#include "blocks/tuntap_pdu.h"
#include "blocks/uchar_to_float.h"
#include "blocks/udp_sink.h"
#include "blocks/udp_source.h"
#include "blocks/unpack_k_bits_bb.h"
#include "blocks/unpacked_to_packed_bb.h"
#include "blocks/unpacked_to_packed_ss.h"
#include "blocks/unpacked_to_packed_ii.h"
#include "blocks/vco_f.h"
#include "blocks/xor_bb.h"
#include "blocks/xor_ss.h"
#include "blocks/xor_ii.h"
%}

%include "blocks/nlog10_ff.h"
%include "blocks/not_bb.h"
%include "blocks/not_ss.h"
%include "blocks/not_ii.h"
%include "blocks/probe_signal_b.h"
%include "blocks/probe_signal_s.h"
%include "blocks/probe_signal_i.h"
%include "blocks/probe_signal_f.h"
%include "blocks/probe_signal_c.h"
%include "blocks/probe_signal_vb.h"
%include "blocks/probe_signal_vs.h"
%include "blocks/probe_signal_vi.h"
%include "blocks/probe_signal_vf.h"
%include "blocks/probe_signal_vc.h"
%include "blocks/or_bb.h"
%include "blocks/or_ss.h"
%include "blocks/or_ii.h"
%include "blocks/pack_k_bits_bb.h"
%include "blocks/packed_to_unpacked_bb.h"
%include "blocks/packed_to_unpacked_ss.h"
%include "blocks/packed_to_unpacked_ii.h"
%include "blocks/patterned_interleaver.h"
%include "blocks/pdu_to_tagged_stream.h"
%include "blocks/peak_detector_fb.h"
%include "blocks/peak_detector_ib.h"
%include "blocks/peak_detector_sb.h"
%include "blocks/peak_detector2_fb.h"
%include "blocks/plateau_detector_fb.h"
%include "blocks/probe_rate.h"
%include "blocks/regenerate_bb.h"
%include "blocks/repack_bits_bb.h"
%include "blocks/repeat.h"
%include "blocks/rms_cf.h"
%include "blocks/rms_ff.h"
%include "blocks/sample_and_hold_bb.h"
%include "blocks/sample_and_hold_ss.h"
%include "blocks/sample_and_hold_ii.h"
%include "blocks/sample_and_hold_ff.h"
%include "blocks/short_to_char.h"
%include "blocks/short_to_float.h"
%include "blocks/socket_pdu.h"
%include "blocks/stretch_ff.h"
%include "blocks/sub_ff.h"
%include "blocks/sub_ss.h"
%include "blocks/sub_ii.h"
%include "blocks/sub_cc.h"
%include "blocks/tagged_stream_mux.h"
%include "blocks/tagged_stream_to_pdu.h"
%include "blocks/threshold_ff.h"
%include "blocks/transcendental.h"
%include "blocks/tuntap_pdu.h"
%include "blocks/uchar_to_float.h"
%include "blocks/udp_sink.h"
%include "blocks/udp_source.h"
%include "blocks/unpack_k_bits_bb.h"
%include "blocks/unpacked_to_packed_bb.h"
%include "blocks/unpacked_to_packed_ss.h"
%include "blocks/unpacked_to_packed_ii.h"
%include "blocks/vco_f.h"
%include "blocks/xor_bb.h"
%include "blocks/xor_ss.h"
%include "blocks/xor_ii.h"

GR_SWIG_BLOCK_MAGIC2(blocks, nlog10_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, not_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, not_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, not_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, patterned_interleaver);
GR_SWIG_BLOCK_MAGIC2(blocks, pack_k_bits_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, packed_to_unpacked_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, packed_to_unpacked_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, packed_to_unpacked_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, peak_detector_fb);
GR_SWIG_BLOCK_MAGIC2(blocks, peak_detector_ib);
GR_SWIG_BLOCK_MAGIC2(blocks, peak_detector_sb);
GR_SWIG_BLOCK_MAGIC2(blocks, peak_detector2_fb);
GR_SWIG_BLOCK_MAGIC2(blocks, plateau_detector_fb);
GR_SWIG_BLOCK_MAGIC2(blocks, pdu_to_tagged_stream);
GR_SWIG_BLOCK_MAGIC2(blocks, probe_rate);
GR_SWIG_BLOCK_MAGIC2(blocks, or_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, or_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, or_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, probe_signal_b);
GR_SWIG_BLOCK_MAGIC2(blocks, probe_signal_s);
GR_SWIG_BLOCK_MAGIC2(blocks, probe_signal_i);
GR_SWIG_BLOCK_MAGIC2(blocks, probe_signal_f);
GR_SWIG_BLOCK_MAGIC2(blocks, probe_signal_c);
GR_SWIG_BLOCK_MAGIC2(blocks, probe_signal_vb);
GR_SWIG_BLOCK_MAGIC2(blocks, probe_signal_vs);
GR_SWIG_BLOCK_MAGIC2(blocks, probe_signal_vi);
GR_SWIG_BLOCK_MAGIC2(blocks, probe_signal_vf);
GR_SWIG_BLOCK_MAGIC2(blocks, probe_signal_vc);
GR_SWIG_BLOCK_MAGIC2(blocks, regenerate_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, repack_bits_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, repeat);
GR_SWIG_BLOCK_MAGIC2(blocks, rms_cf);
GR_SWIG_BLOCK_MAGIC2(blocks, rms_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, sample_and_hold_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, sample_and_hold_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, sample_and_hold_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, sample_and_hold_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, short_to_char);
GR_SWIG_BLOCK_MAGIC2(blocks, short_to_float);
GR_SWIG_BLOCK_MAGIC2(blocks, socket_pdu);
GR_SWIG_BLOCK_MAGIC2(blocks, stretch_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, sub_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, sub_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, sub_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, sub_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, tagged_stream_mux);
GR_SWIG_BLOCK_MAGIC2(blocks, tagged_stream_to_pdu);
GR_SWIG_BLOCK_MAGIC2(blocks, threshold_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, transcendental);
GR_SWIG_BLOCK_MAGIC2(blocks, tuntap_pdu);
GR_SWIG_BLOCK_MAGIC2(blocks, uchar_to_float);
GR_SWIG_BLOCK_MAGIC2(blocks, udp_sink);
GR_SWIG_BLOCK_MAGIC2(blocks, udp_source);
GR_SWIG_BLOCK_MAGIC2(blocks, unpack_k_bits_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, unpacked_to_packed_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, unpacked_to_packed_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, unpacked_to_packed_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, vco_f);
GR_SWIG_BLOCK_MAGIC2(blocks, xor_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, xor_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, xor_ii);
