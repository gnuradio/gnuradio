/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
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
%include "blocks_swig5_doc.i"

%{
#include "gnuradio/blocks/pdu.h"
#include "gnuradio/blocks/random_pdu.h"
#include "gnuradio/blocks/regenerate_bb.h"
#include "gnuradio/blocks/repack_bits_bb.h"
#include "gnuradio/blocks/repeat.h"
#include "gnuradio/blocks/rms_cf.h"
#include "gnuradio/blocks/rms_ff.h"
#include "gnuradio/blocks/rotator_cc.h"
#include "gnuradio/blocks/sample_and_hold.h"
#include "gnuradio/blocks/short_to_char.h"
#include "gnuradio/blocks/short_to_float.h"
#include "gnuradio/blocks/socket_pdu.h"
#include "gnuradio/blocks/stretch_ff.h"
#include "gnuradio/blocks/sub.h"
%}

%include "gnuradio/blocks/random_pdu.h"
%include "gnuradio/blocks/regenerate_bb.h"
%include "gnuradio/blocks/repack_bits_bb.h"
%include "gnuradio/blocks/repeat.h"
%include "gnuradio/blocks/rms_cf.h"
%include "gnuradio/blocks/rms_ff.h"
%include "gnuradio/blocks/rotator_cc.h"
%include "gnuradio/blocks/sample_and_hold.h"
%include "gnuradio/blocks/short_to_char.h"
%include "gnuradio/blocks/short_to_float.h"
%include "gnuradio/blocks/socket_pdu.h"
%include "gnuradio/blocks/stretch_ff.h"
%include "gnuradio/blocks/sub.h"

GR_SWIG_BLOCK_MAGIC2(blocks, random_pdu);
GR_SWIG_BLOCK_MAGIC2(blocks, regenerate_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, repack_bits_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, repeat);
GR_SWIG_BLOCK_MAGIC2(blocks, rms_cf);
GR_SWIG_BLOCK_MAGIC2(blocks, rms_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, rotator_cc);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, sample_and_hold_bb, sample_and_hold<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, sample_and_hold_ss, sample_and_hold<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, sample_and_hold_ii, sample_and_hold<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, sample_and_hold_ff, sample_and_hold<float>);
GR_SWIG_BLOCK_MAGIC2(blocks, short_to_char);
GR_SWIG_BLOCK_MAGIC2(blocks, short_to_float);
GR_SWIG_BLOCK_MAGIC2(blocks, socket_pdu);
GR_SWIG_BLOCK_MAGIC2(blocks, stretch_ff);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, sub_ff, sub<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, sub_ss, sub<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, sub_ii, sub<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, sub_cc, sub<gr_complex>);
