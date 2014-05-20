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
%include "gnuradio/blocks/pdu.h"

%include <gnuradio/endianness.h>

//load generated python docstrings
%include "blocks_swig5_doc.i"

%{
#include "gnuradio/blocks/random_pdu.h"
#include "gnuradio/blocks/regenerate_bb.h"
#include "gnuradio/blocks/repack_bits_bb.h"
#include "gnuradio/blocks/repeat.h"
#include "gnuradio/blocks/rms_cf.h"
#include "gnuradio/blocks/rms_ff.h"
#include "gnuradio/blocks/rotator_cc.h"
#include "gnuradio/blocks/sample_and_hold_bb.h"
#include "gnuradio/blocks/sample_and_hold_ss.h"
#include "gnuradio/blocks/sample_and_hold_ii.h"
#include "gnuradio/blocks/sample_and_hold_ff.h"
#include "gnuradio/blocks/short_to_char.h"
#include "gnuradio/blocks/short_to_float.h"
#include "gnuradio/blocks/socket_pdu.h"
#include "gnuradio/blocks/stretch_ff.h"
#include "gnuradio/blocks/sub_ff.h"
#include "gnuradio/blocks/sub_ss.h"
#include "gnuradio/blocks/sub_ii.h"
#include "gnuradio/blocks/sub_cc.h"
#include "gnuradio/blocks/tag_gate.h"
#include "gnuradio/blocks/tagged_stream_mux.h"
#include "gnuradio/blocks/tagged_stream_multiply_length.h"
#include "gnuradio/blocks/tagged_stream_to_pdu.h"
#include "gnuradio/blocks/tags_strobe.h"
#include "gnuradio/blocks/threshold_ff.h"
#include "gnuradio/blocks/transcendental.h"
#include "gnuradio/blocks/tuntap_pdu.h"
#include "gnuradio/blocks/uchar_to_float.h"
#include "gnuradio/blocks/udp_sink.h"
#include "gnuradio/blocks/udp_source.h"
#include "gnuradio/blocks/unpack_k_bits.h"
#include "gnuradio/blocks/unpack_k_bits_bb.h"
#include "gnuradio/blocks/unpacked_to_packed_bb.h"
#include "gnuradio/blocks/unpacked_to_packed_ss.h"
#include "gnuradio/blocks/unpacked_to_packed_ii.h"
#include "gnuradio/blocks/vco_f.h"
#include "gnuradio/blocks/vco_c.h"
#include "gnuradio/blocks/xor_bb.h"
#include "gnuradio/blocks/xor_ss.h"
#include "gnuradio/blocks/xor_ii.h"
%}

%include "gnuradio/blocks/random_pdu.h"
%include "gnuradio/blocks/regenerate_bb.h"
%include "gnuradio/blocks/repack_bits_bb.h"
%include "gnuradio/blocks/repeat.h"
%include "gnuradio/blocks/rms_cf.h"
%include "gnuradio/blocks/rms_ff.h"
%include "gnuradio/blocks/rotator_cc.h"
%include "gnuradio/blocks/sample_and_hold_bb.h"
%include "gnuradio/blocks/sample_and_hold_ss.h"
%include "gnuradio/blocks/sample_and_hold_ii.h"
%include "gnuradio/blocks/sample_and_hold_ff.h"
%include "gnuradio/blocks/short_to_char.h"
%include "gnuradio/blocks/short_to_float.h"
%include "gnuradio/blocks/socket_pdu.h"
%include "gnuradio/blocks/stretch_ff.h"
%include "gnuradio/blocks/sub_ff.h"
%include "gnuradio/blocks/sub_ss.h"
%include "gnuradio/blocks/sub_ii.h"
%include "gnuradio/blocks/sub_cc.h"
%include "gnuradio/blocks/tag_gate.h"
%include "gnuradio/blocks/tagged_stream_mux.h"
%include "gnuradio/blocks/tagged_stream_multiply_length.h"
%include "gnuradio/blocks/tagged_stream_to_pdu.h"
%include "gnuradio/blocks/tags_strobe.h"
%include "gnuradio/blocks/threshold_ff.h"
%include "gnuradio/blocks/transcendental.h"
%include "gnuradio/blocks/tuntap_pdu.h"
%include "gnuradio/blocks/uchar_to_float.h"
%include "gnuradio/blocks/udp_sink.h"
%include "gnuradio/blocks/udp_source.h"
%include "gnuradio/blocks/unpack_k_bits.h"
%include "gnuradio/blocks/unpack_k_bits_bb.h"
%include "gnuradio/blocks/unpacked_to_packed_bb.h"
%include "gnuradio/blocks/unpacked_to_packed_ss.h"
%include "gnuradio/blocks/unpacked_to_packed_ii.h"
%include "gnuradio/blocks/vco_f.h"
%include "gnuradio/blocks/vco_c.h"
%include "gnuradio/blocks/xor_bb.h"
%include "gnuradio/blocks/xor_ss.h"
%include "gnuradio/blocks/xor_ii.h"

GR_SWIG_BLOCK_MAGIC2(blocks, random_pdu);
GR_SWIG_BLOCK_MAGIC2(blocks, regenerate_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, repack_bits_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, repeat);
GR_SWIG_BLOCK_MAGIC2(blocks, rms_cf);
GR_SWIG_BLOCK_MAGIC2(blocks, rms_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, rotator_cc);
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
GR_SWIG_BLOCK_MAGIC2(blocks, tag_gate);
GR_SWIG_BLOCK_MAGIC2(blocks, tagged_stream_mux);
GR_SWIG_BLOCK_MAGIC2(blocks, tagged_stream_multiply_length);
GR_SWIG_BLOCK_MAGIC2(blocks, tagged_stream_to_pdu);
GR_SWIG_BLOCK_MAGIC2(blocks, tags_strobe);
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
GR_SWIG_BLOCK_MAGIC2(blocks, vco_c);
GR_SWIG_BLOCK_MAGIC2(blocks, xor_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, xor_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, xor_ii);
