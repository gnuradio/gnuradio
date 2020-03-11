/* -*- c++ -*- */
/*
 * Copyright 2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
#include "gnuradio/blocks/unpacked_to_packed.h"
#include "gnuradio/blocks/vco_f.h"
#include "gnuradio/blocks/vco_c.h"
#include "gnuradio/blocks/xor_blk.h"
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
%include "gnuradio/blocks/unpacked_to_packed.h"
%include "gnuradio/blocks/vco_f.h"
%include "gnuradio/blocks/vco_c.h"
%include "gnuradio/blocks/xor_blk.h"

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
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, unpacked_to_packed_bb, unpacked_to_packed<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, unpacked_to_packed_ss, unpacked_to_packed<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, unpacked_to_packed_ii, unpacked_to_packed<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2(blocks, vco_f);
GR_SWIG_BLOCK_MAGIC2(blocks, vco_c);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, xor_bb, xor_blk<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, xor_ss, xor_blk<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, xor_ii, xor_blk<std::int32_t>);
