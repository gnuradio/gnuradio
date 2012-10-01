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
#include "blocks/and_bb.h"
#include "blocks/and_ss.h"
#include "blocks/and_ii.h"
#include "blocks/and_const_bb.h"
#include "blocks/and_const_ss.h"
#include "blocks/and_const_ii.h"
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
#include "blocks/deinterleave.h"
#include "blocks/divide_ff.h"
#include "blocks/divide_ss.h"
#include "blocks/divide_ii.h"
#include "blocks/divide_cc.h"
#include "blocks/file_source.h"
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
#include "blocks/interleave.h"
#include "blocks/interleaved_short_to_complex.h"
#include "blocks/keep_m_in_n.h"
#include "blocks/keep_one_in_n.h"
#include "blocks/multiply_ss.h"
#include "blocks/multiply_ii.h"
#include "blocks/multiply_ff.h"
#include "blocks/multiply_cc.h"
#include "blocks/multiply_conjugate_cc.h"
#include "blocks/multiply_const_ss.h"
#include "blocks/multiply_const_ii.h"
#include "blocks/multiply_const_ff.h"
#include "blocks/multiply_const_cc.h"
#include "blocks/multiply_const_vss.h"
#include "blocks/multiply_const_vii.h"
#include "blocks/multiply_const_vff.h"
#include "blocks/multiply_const_vcc.h"
#include "blocks/nlog10_ff.h"
#include "blocks/not_bb.h"
#include "blocks/not_ss.h"
#include "blocks/not_ii.h"
#include "blocks/or_bb.h"
#include "blocks/or_ss.h"
#include "blocks/or_ii.h"
#include "blocks/repeat.h"
#include "blocks/short_to_char.h"
#include "blocks/short_to_float.h"
#include "blocks/stream_mux.h"
#include "blocks/stream_to_streams.h"
#include "blocks/stream_to_vector.h"
#include "blocks/streams_to_stream.h"
#include "blocks/streams_to_vector.h"
#include "blocks/sub_ff.h"
#include "blocks/sub_ss.h"
#include "blocks/sub_ii.h"
#include "blocks/sub_cc.h"
#include "blocks/uchar_to_float.h"
#include "blocks/vector_to_stream.h"
#include "blocks/vector_to_streams.h"
#include "blocks/xor_bb.h"
#include "blocks/xor_ss.h"
#include "blocks/xor_ii.h"
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
%include "blocks/and_bb.h"
%include "blocks/and_ss.h"
%include "blocks/and_ii.h"
%include "blocks/and_const_bb.h"
%include "blocks/and_const_ss.h"
%include "blocks/and_const_ii.h"
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
%include "blocks/deinterleave.h"
%include "blocks/file_source.h"
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
%include "blocks/interleave.h"
%include "blocks/interleaved_short_to_complex.h"
%include "blocks/keep_m_in_n.h"
%include "blocks/keep_one_in_n.h"
%include "blocks/multiply_ss.h"
%include "blocks/multiply_ii.h"
%include "blocks/multiply_ff.h"
%include "blocks/multiply_cc.h"
%include "blocks/multiply_conjugate_cc.h"
%include "blocks/multiply_const_ss.h"
%include "blocks/multiply_const_ii.h"
%include "blocks/multiply_const_ff.h"
%include "blocks/multiply_const_cc.h"
%include "blocks/multiply_const_vss.h"
%include "blocks/multiply_const_vii.h"
%include "blocks/multiply_const_vff.h"
%include "blocks/multiply_const_vcc.h"
%include "blocks/nlog10_ff.h"
%include "blocks/not_bb.h"
%include "blocks/not_ss.h"
%include "blocks/not_ii.h"
%include "blocks/or_bb.h"
%include "blocks/or_ss.h"
%include "blocks/or_ii.h"
%include "blocks/repeat.h"
%include "blocks/short_to_char.h"
%include "blocks/short_to_float.h"
%include "blocks/stream_mux.h"
%include "blocks/stream_to_streams.h"
%include "blocks/stream_to_vector.h"
%include "blocks/streams_to_stream.h"
%include "blocks/streams_to_vector.h"
%include "blocks/sub_ff.h"
%include "blocks/sub_ss.h"
%include "blocks/sub_ii.h"
%include "blocks/sub_cc.h"
%include "blocks/uchar_to_float.h"
%include "blocks/vector_to_stream.h"
%include "blocks/vector_to_streams.h"
%include "blocks/xor_bb.h"
%include "blocks/xor_ss.h"
%include "blocks/xor_ii.h"

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
GR_SWIG_BLOCK_MAGIC2(blocks, and_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, and_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, and_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, and_const_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, and_const_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, and_const_ii);
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
GR_SWIG_BLOCK_MAGIC2(blocks, deinterleave);
GR_SWIG_BLOCK_MAGIC2(blocks, divide_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, divide_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, divide_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, divide_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, file_source);
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
GR_SWIG_BLOCK_MAGIC2(blocks, interleave);
GR_SWIG_BLOCK_MAGIC2(blocks, interleaved_short_to_complex);
GR_SWIG_BLOCK_MAGIC2(blocks, keep_m_in_n);
GR_SWIG_BLOCK_MAGIC2(blocks, keep_one_in_n);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_conjugate_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_vss);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_vii);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_vff);
GR_SWIG_BLOCK_MAGIC2(blocks, multiply_const_vcc);
GR_SWIG_BLOCK_MAGIC2(blocks, nlog10_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, not_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, not_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, not_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, or_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, or_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, or_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, repeat);
GR_SWIG_BLOCK_MAGIC2(blocks, short_to_char);
GR_SWIG_BLOCK_MAGIC2(blocks, short_to_float);
GR_SWIG_BLOCK_MAGIC2(blocks, stream_mux);
GR_SWIG_BLOCK_MAGIC2(blocks, stream_to_streams);
GR_SWIG_BLOCK_MAGIC2(blocks, stream_to_vector);
GR_SWIG_BLOCK_MAGIC2(blocks, streams_to_stream);
GR_SWIG_BLOCK_MAGIC2(blocks, streams_to_vector);
GR_SWIG_BLOCK_MAGIC2(blocks, sub_ff);
GR_SWIG_BLOCK_MAGIC2(blocks, sub_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, sub_ii);
GR_SWIG_BLOCK_MAGIC2(blocks, sub_cc);
GR_SWIG_BLOCK_MAGIC2(blocks, uchar_to_float);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_to_stream);
GR_SWIG_BLOCK_MAGIC2(blocks, vector_to_streams);
GR_SWIG_BLOCK_MAGIC2(blocks, xor_bb);
GR_SWIG_BLOCK_MAGIC2(blocks, xor_ss);
GR_SWIG_BLOCK_MAGIC2(blocks, xor_ii);
