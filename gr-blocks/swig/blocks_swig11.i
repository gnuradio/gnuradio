/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#define BLOCKS_API

%include "gnuradio.i"

//load generated python docstrings
%include "blocks_swig11_doc.i"

%{
#include "gnuradio/blocks/math_functions.h"
%}

%include "gnuradio/blocks/math_functions.h"

GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_cos_complex_float, math_functions_complex_float<std::complex<float>, std::cos>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_cosh_complex_float, math_functions_complex_float<std::complex<float>, std::cosh>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_exp_complex_float, math_functions_complex_float<std::complex<float>, std::exp>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_log_complex_float, math_functions_complex_float<std::complex<float>, std::log>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_sin_complex_float, math_functions_complex_float<std::complex<float>, std::sin>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_sinh_complex_float, math_functions_complex_float<std::complex<float>, std::sinh>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_sqrt_complex_float, math_functions_complex_float<std::complex<float>, std::sqrt>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_tan_complex_float, math_functions_complex_float<std::complex<float>, std::tan>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_tanh_complex_float, math_functions_complex_float<std::complex<float>, std::tanh>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_acos_complex_float, math_functions_complex_float<std::complex<float>, std::acos>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_acosh_complex_float, math_functions_complex_float<std::complex<float>, std::acosh>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_asin_complex_float, math_functions_complex_float<std::complex<float>, std::asin>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_asinh_complex_float, math_functions_complex_float<std::complex<float>, std::asinh>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_atan_complex_float, math_functions_complex_float<std::complex<float>, std::atan>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_atanh_complex_float, math_functions_complex_float<std::complex<float>, std::atanh>);

GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_cos_float, math_functions_float<float, cos>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_cosh_float, math_functions_float<float, cosh>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_exp_float, math_functions_float<float, exp>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_log_float, math_functions_float<float, log>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_sin_float, math_functions_float<float, sin>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_sinh_float, math_functions_float<float, sinh>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_sqrt_float, math_functions_float<float, sqrt>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_tan_float, math_functions_float<float, tan>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_tanh_float, math_functions_float<float, tanh>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_acos_float, math_functions_float<float, acos>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_acosh_float, math_functions_float<float, acosh>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_asin_float, math_functions_float<float, asin>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_asinh_float, math_functions_float<float, asinh>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_atan_float, math_functions_float<float, atan>);
GR_SWIG_BLOCK_MAGIC2_TMPL(blocks, math_functions_atanh_float, math_functions_float<float, atanh>);