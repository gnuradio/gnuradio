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
%include "blocks_swig10_doc.i"

%{
#include "gnuradio/blocks/min_blk.h"
#include "gnuradio/blocks/moving_average.h"
#include "gnuradio/blocks/nlog10_ff.h"
#include "gnuradio/blocks/not_blk.h"
#include "gnuradio/blocks/or_blk.h"
#include "gnuradio/blocks/patterned_interleaver.h"
#include "gnuradio/blocks/pack_k_bits_bb.h"
#include "gnuradio/blocks/packed_to_unpacked.h"
%}

%include "gnuradio/blocks/min_blk.h"
%include "gnuradio/blocks/moving_average.h"
%include "gnuradio/blocks/nlog10_ff.h"
%include "gnuradio/blocks/not_blk.h"
%include "gnuradio/blocks/or_blk.h"
%include "gnuradio/blocks/patterned_interleaver.h"
%include "gnuradio/blocks/pack_k_bits_bb.h"
%include "gnuradio/blocks/packed_to_unpacked.h"

GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, min_ff, min_blk<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, min_ii, min_blk<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, min_ss, min_blk<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, moving_average_cc, moving_average<gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, moving_average_ff, moving_average<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, moving_average_ii, moving_average<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, moving_average_ss, moving_average<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2(blocks, nlog10_ff);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, not_bb, not_blk<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, not_ss, not_blk<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, not_ii, not_blk<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, or_bb, or_blk<char>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, or_ss, or_blk<short>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, or_ii, or_blk<int>);
GR_SWIG_BLOCK_MAGIC2(blocks, patterned_interleaver);
GR_SWIG_BLOCK_MAGIC2(blocks, pack_k_bits_bb);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, packed_to_unpacked_bb, packed_to_unpacked<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, packed_to_unpacked_ss, packed_to_unpacked<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, packed_to_unpacked_ii, packed_to_unpacked<std::int32_t>);
