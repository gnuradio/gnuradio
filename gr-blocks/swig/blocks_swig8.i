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
