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

//load generated python docstrings
%include "blocks_swig8_doc.i"

%{
#include "gnuradio/blocks/abs_blk.h"
#include "gnuradio/blocks/add_blk.h"
#include "gnuradio/blocks/add_const_bb.h"
#include "gnuradio/blocks/add_const_ff.h"
#include "gnuradio/blocks/add_const_ss.h"
#include "gnuradio/blocks/add_const_ii.h"
#include "gnuradio/blocks/add_const_cc.h"
#include "gnuradio/blocks/add_const_v.h"
#include "gnuradio/blocks/and_blk.h"
%}

%include "gnuradio/blocks/abs_blk.h"
%include "gnuradio/blocks/add_blk.h"
%include "gnuradio/blocks/add_const_bb.h"
%include "gnuradio/blocks/add_const_ff.h"
%include "gnuradio/blocks/add_const_ss.h"
%include "gnuradio/blocks/add_const_ii.h"
%include "gnuradio/blocks/add_const_cc.h"
%include "gnuradio/blocks/add_const_v.h"
%include "gnuradio/blocks/and_blk.h"

GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, abs_ff, abs_blk<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, abs_ss, abs_blk<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, abs_ii, abs_blk<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, add_ff, add_blk<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, add_ss, add_blk<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, add_ii, add_blk<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, add_cc, add_blk<gr_complex>);
GR_SWIG_BLOCK_MAGIC2(blocks, add_const_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, add_const_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, add_const_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, add_const_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, add_const_cc);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, add_const_vbb, add_const_v<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, add_const_vff, add_const_v<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, add_const_vss, add_const_v<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, add_const_vii, add_const_v<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, add_const_vcc, add_const_v<gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, and_bb, and_blk<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, and_ss, and_blk<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, and_ii, and_blk<std::int32_t>);
