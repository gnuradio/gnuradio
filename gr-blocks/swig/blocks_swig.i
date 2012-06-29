/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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
%include "blocks_swig_doc.i"

%{
#include "blocks/add_ff.h"
#include "blocks/add_ss.h"
#include "blocks/add_ii.h"
#include "blocks/add_cc.h"
#include "blocks/add_const_ff.h"
#include "blocks/add_const_ss.h"
#include "blocks/add_const_ii.h"
#include "blocks/add_const_cc.h"
#include "blocks/add_const_vff.h"
#include "blocks/add_const_vss.h"
#include "blocks/add_const_vii.h"
#include "blocks/add_const_vcc.h"
#include "blocks/char_to_float.h"
#include "blocks/char_to_short.h"
#include "blocks/complex_to_interleaved_short.h"
#include "blocks/complex_to_float.h"
#include "blocks/complex_to_real.h"
#include "blocks/complex_to_imag.h"
#include "blocks/complex_to_mag.h"
#include "blocks/complex_to_mag_squared.h"
#include "blocks/complex_to_arg.h"
#include "blocks/conjugate_cc.h"
#include "blocks/divide_ff.h"
#include "blocks/divide_ss.h"
#include "blocks/divide_ii.h"
#include "blocks/divide_cc.h"
#include "blocks/float_to_char.h"
#include "blocks/float_to_complex.h"
#include "blocks/float_to_int.h"
#include "blocks/float_to_short.h"
#include "blocks/float_to_uchar.h"
#include "blocks/int_to_float.h"
#include "blocks/integrate_ss.h"
#include "blocks/integrate_ii.h"
#include "blocks/integrate_ff.h"
#include "blocks/integrate_cc.h"
#include "blocks/interleaved_short_to_complex.h"
#include "blocks/multiply_ss.h"
#include "blocks/multiply_ii.h"
#include "blocks/multiply_ff.h"
#include "blocks/multiply_cc.h"
#include "blocks/multiply_const_ss.h"
#include "blocks/multiply_const_ii.h"
#include "blocks/multiply_const_ff.h"
#include "blocks/multiply_const_cc.h"
#include "blocks/multiply_const_vss.h"
#include "blocks/multiply_const_vii.h"
#include "blocks/multiply_const_vff.h"
#include "blocks/multiply_const_vcc.h"
#include "blocks/short_to_char.h"
#include "blocks/short_to_float.h"
#include "blocks/sub_ff.h"
#include "blocks/sub_ss.h"
#include "blocks/sub_ii.h"
#include "blocks/sub_cc.h"
#include "blocks/uchar_to_float.h"
%}

%include "blocks/add_ff.h"
%include "blocks/add_ss.h"
%include "blocks/add_ii.h"
%include "blocks/add_cc.h"
%include "blocks/add_const_ff.h"
%include "blocks/add_const_ss.h"
%include "blocks/add_const_ii.h"
%include "blocks/add_const_cc.h"
%include "blocks/add_const_vff.h"
%include "blocks/add_const_vss.h"
%include "blocks/add_const_vii.h"
%include "blocks/add_const_vcc.h"
%include "blocks/char_to_float.h"
%include "blocks/char_to_short.h"
%include "blocks/complex_to_interleaved_short.h"
%include "blocks/complex_to_float.h"
%include "blocks/complex_to_real.h"
%include "blocks/complex_to_imag.h"
%include "blocks/complex_to_mag.h"
%include "blocks/complex_to_mag_squared.h"
%include "blocks/complex_to_arg.h"
%include "blocks/conjugate_cc.h"
%include "blocks/divide_ff.h"
%include "blocks/divide_ss.h"
%include "blocks/divide_ii.h"
%include "blocks/divide_cc.h"
%include "blocks/float_to_char.h"
%include "blocks/float_to_complex.h"
%include "blocks/float_to_int.h"
%include "blocks/float_to_short.h"
%include "blocks/float_to_uchar.h"
%include "blocks/int_to_float.h"
%include "blocks/integrate_ss.h"
%include "blocks/integrate_ii.h"
%include "blocks/integrate_ff.h"
%include "blocks/integrate_cc.h"
%include "blocks/interleaved_short_to_complex.h"
%include "blocks/multiply_ss.h"
%include "blocks/multiply_ii.h"
%include "blocks/multiply_ff.h"
%include "blocks/multiply_cc.h"
%include "blocks/multiply_const_ss.h"
%include "blocks/multiply_const_ii.h"
%include "blocks/multiply_const_ff.h"
%include "blocks/multiply_const_cc.h"
%include "blocks/multiply_const_vss.h"
%include "blocks/multiply_const_vii.h"
%include "blocks/multiply_const_vff.h"
%include "blocks/multiply_const_vcc.h"
%include "blocks/short_to_char.h"
%include "blocks/short_to_float.h"
%include "blocks/sub_ff.h"
%include "blocks/sub_ss.h"
%include "blocks/sub_ii.h"
%include "blocks/sub_cc.h"
%include "blocks/uchar_to_float.h"

GR_SWIG_BLOCK_MAGIC2(blocks, add_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, add_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, add_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, add_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, add_const_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, add_const_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, add_const_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, add_const_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, add_const_vff);
GR_SWIG_BLOCK_MAGIC2(blocks, add_const_vss);
GR_SWIG_BLOCK_MAGIC2(blocks, add_const_vii);
GR_SWIG_BLOCK_MAGIC2(blocks, add_const_vcc);
GR_SWIG_BLOCK_MAGIC2(blocks, char_to_float);
GR_SWIG_BLOCK_MAGIC2(blocks, char_to_short);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_interleaved_short);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_float);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_real);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_imag);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_mag);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_mag_squared);
GR_SWIG_BLOCK_MAGIC2(blocks, complex_to_arg);
GR_SWIG_BLOCK_MAGIC2(blocks, conjugate_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, divide_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, divide_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, divide_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, divide_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, float_to_char);
GR_SWIG_BLOCK_MAGIC2(blocks, float_to_complex);
GR_SWIG_BLOCK_MAGIC2(blocks, float_to_int);
GR_SWIG_BLOCK_MAGIC2(blocks, float_to_short);
GR_SWIG_BLOCK_MAGIC2(blocks, float_to_uchar);
GR_SWIG_BLOCK_MAGIC2(blocks, int_to_float);
GR_SWIG_BLOCK_MAGIC2(blocks, integrate_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, integrate_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, integrate_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, integrate_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, interleaved_short_to_complex);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_vss);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_vii);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_vff);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_vcc);
GR_SWIG_BLOCK_MAGIC2(blocks, short_to_char);
GR_SWIG_BLOCK_MAGIC2(blocks, short_to_float);
GR_SWIG_BLOCK_MAGIC2(blocks, sub_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, sub_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, sub_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, sub_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, uchar_to_float);
