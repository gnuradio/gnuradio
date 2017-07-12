/* -*- c++ -*- */
/*
 * Copyright 2017 Free Software Foundation, Inc.
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
%include "blocks_swig6_doc.i"

%{
#include "gnuradio/blocks/tag_gate.h"
#include "gnuradio/blocks/tag_share.h"
#include "gnuradio/blocks/tagged_stream_align.h"
#include "gnuradio/blocks/tagged_stream_mux.h"
#include "gnuradio/blocks/tagged_stream_multiply_length.h"
#include "gnuradio/blocks/tagged_stream_to_pdu.h"
#include "gnuradio/blocks/tags_strobe.h"
#include "gnuradio/blocks/tcp_server_sink.h"
#include "gnuradio/blocks/test_tag_variable_rate_ff.h"
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

%include "gnuradio/blocks/tag_gate.h"
%include "gnuradio/blocks/tag_share.h"
%include "gnuradio/blocks/tagged_stream_align.h"
%include "gnuradio/blocks/tagged_stream_mux.h"
%include "gnuradio/blocks/tagged_stream_multiply_length.h"
%include "gnuradio/blocks/tagged_stream_to_pdu.h"
%include "gnuradio/blocks/tags_strobe.h"
%include "gnuradio/blocks/tcp_server_sink.h"
%include "gnuradio/blocks/test_tag_variable_rate_ff.h"
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

GR_SWIG_BLOCK_MAGIC2(blocks, tag_gate);
GR_SWIG_BLOCK_MAGIC2(blocks, tag_share);
GR_SWIG_BLOCK_MAGIC2(blocks, tagged_stream_align);
GR_SWIG_BLOCK_MAGIC2(blocks, tagged_stream_mux);
GR_SWIG_BLOCK_MAGIC2(blocks, tagged_stream_multiply_length);
GR_SWIG_BLOCK_MAGIC2(blocks, tagged_stream_to_pdu);
GR_SWIG_BLOCK_MAGIC2(blocks, tags_strobe);
GR_SWIG_BLOCK_MAGIC2(blocks, tcp_server_sink);
GR_SWIG_BLOCK_MAGIC2(blocks, test_tag_variable_rate_ff);
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
