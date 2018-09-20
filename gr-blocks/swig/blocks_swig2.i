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
#define GR_RUNTIME_API

%include "gnuradio.i"

//load generated python docstrings
%include "blocks_swig2_doc.i"

%include "feval.i"

%{
#include "gnuradio/blocks/and_const.h"
#include "gnuradio/blocks/argmax.h"
#include "gnuradio/blocks/bin_statistics_f.h"
#include "gnuradio/blocks/burst_tagger.h"
#include "gnuradio/blocks/char_to_float.h"
#include "gnuradio/blocks/char_to_short.h"
#include "gnuradio/blocks/check_lfsr_32k_s.h"
#include "gnuradio/blocks/complex_to_interleaved_char.h"
#include "gnuradio/blocks/complex_to_interleaved_short.h"
#include "gnuradio/blocks/complex_to_float.h"
#include "gnuradio/blocks/complex_to_magphase.h"
#include "gnuradio/blocks/complex_to_real.h"
#include "gnuradio/blocks/complex_to_imag.h"
#include "gnuradio/blocks/complex_to_mag.h"
#include "gnuradio/blocks/complex_to_mag_squared.h"
#include "gnuradio/blocks/complex_to_arg.h"
#include "gnuradio/blocks/conjugate_cc.h"
%}

%include "gnuradio/blocks/and_const.h"
%include "gnuradio/blocks/argmax.h"
%include "gnuradio/blocks/char_to_float.h"
%include "gnuradio/blocks/bin_statistics_f.h"
%include "gnuradio/blocks/burst_tagger.h"
%include "gnuradio/blocks/char_to_short.h"
%include "gnuradio/blocks/check_lfsr_32k_s.h"
%include "gnuradio/blocks/complex_to_interleaved_char.h"
%include "gnuradio/blocks/complex_to_interleaved_short.h"
%include "gnuradio/blocks/complex_to_float.h"
%include "gnuradio/blocks/complex_to_magphase.h"
%include "gnuradio/blocks/complex_to_real.h"
%include "gnuradio/blocks/complex_to_imag.h"
%include "gnuradio/blocks/complex_to_mag.h"
%include "gnuradio/blocks/complex_to_mag_squared.h"
%include "gnuradio/blocks/complex_to_arg.h"
%include "gnuradio/blocks/conjugate_cc.h"

GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, and_const_bb, and_const<std::uint8_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, and_const_ss, and_const<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, and_const_ii, and_const<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, argmax_fs, argmax<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, argmax_is, argmax<std::int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, argmax_ss, argmax<std::int16_t>);
GR_SWIG_BLOCK_MAGIC2(blocks, bin_statistics_f);
GR_SWIG_BLOCK_MAGIC2(blocks, burst_tagger);
GR_SWIG_BLOCK_MAGIC2(blocks, char_to_float);
GR_SWIG_BLOCK_MAGIC2(blocks, char_to_short);
GR_SWIG_BLOCK_MAGIC2(blocks, check_lfsr_32k_s);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_interleaved_char);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_interleaved_short);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_float);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_magphase);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_real);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_imag);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_mag);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_mag_squared);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_arg);
GR_SWIG_BLOCK_MAGIC2(blocks, conjugate_cc);
