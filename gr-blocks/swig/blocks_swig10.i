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
#include "gnuradio/blocks/min_ff.h"
#include "gnuradio/blocks/min_ii.h"
#include "gnuradio/blocks/min_ss.h"
#include "gnuradio/blocks/moving_average_cc.h"
#include "gnuradio/blocks/moving_average_ff.h"
#include "gnuradio/blocks/moving_average_ii.h"
#include "gnuradio/blocks/moving_average_ss.h"
#include "gnuradio/blocks/nlog10_ff.h"
#include "gnuradio/blocks/not_bb.h"
#include "gnuradio/blocks/not_ss.h"
#include "gnuradio/blocks/not_ii.h"
#include "gnuradio/blocks/or_bb.h"
#include "gnuradio/blocks/or_ss.h"
#include "gnuradio/blocks/or_ii.h"
#include "gnuradio/blocks/patterned_interleaver.h"
#include "gnuradio/blocks/pack_k_bits_bb.h"
#include "gnuradio/blocks/packed_to_unpacked_bb.h"
#include "gnuradio/blocks/packed_to_unpacked_ss.h"
#include "gnuradio/blocks/packed_to_unpacked_ii.h"
%}

%include "gnuradio/blocks/min_ff.h"
%include "gnuradio/blocks/min_ii.h"
%include "gnuradio/blocks/min_ss.h"
%include "gnuradio/blocks/moving_average_cc.h"
%include "gnuradio/blocks/moving_average_ff.h"
%include "gnuradio/blocks/moving_average_ii.h"
%include "gnuradio/blocks/moving_average_ss.h"
%include "gnuradio/blocks/nlog10_ff.h"
%include "gnuradio/blocks/not_bb.h"
%include "gnuradio/blocks/not_ss.h"
%include "gnuradio/blocks/not_ii.h"
%include "gnuradio/blocks/or_bb.h"
%include "gnuradio/blocks/or_ss.h"
%include "gnuradio/blocks/or_ii.h"
%include "gnuradio/blocks/patterned_interleaver.h"
%include "gnuradio/blocks/pack_k_bits_bb.h"
%include "gnuradio/blocks/packed_to_unpacked_bb.h"
%include "gnuradio/blocks/packed_to_unpacked_ss.h"
%include "gnuradio/blocks/packed_to_unpacked_ii.h"

GR_SWIG_BLOCK_MAGIC2(blocks, min_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, min_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, min_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, moving_average_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, moving_average_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, moving_average_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, moving_average_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, nlog10_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, not_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, not_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, not_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, or_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, or_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, or_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, patterned_interleaver);
GR_SWIG_BLOCK_MAGIC2(blocks, pack_k_bits_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, packed_to_unpacked_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, packed_to_unpacked_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, packed_to_unpacked_ii);
