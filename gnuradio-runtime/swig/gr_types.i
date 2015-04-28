/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef SWIG_GR_TYPES_I
#define SWIG_GR_TYPES_I

%include "stdint.i"

%{
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <complex>
#include <string>
#include <stddef.h>		// size_t
#include <stdint.h>
#include <gnuradio/types.h>
%}

%include <std_complex.i>
%include <std_vector.i>
%include <stl.i>

typedef std::complex<float>		gr_complex;
typedef std::complex<double>		gr_complexd;
typedef unsigned long long              uint64_t;
typedef long long                       int64_t;

namespace std {
  %template() vector<unsigned char>;
  %template() vector<signed char>;
  %template() vector<char>;

  %template() vector<unsigned short>;
  %template() vector<short>;

  %template() vector<int>;
  %template() vector<unsigned int>;

  %template() vector<long int>;
  %template() vector<long unsigned int>;

  %template() vector<float>;
  %template() vector<double>;

  %template() vector< vector< unsigned char > >;
  %template() vector< vector< char > >;
  %template() vector< vector< short > >;
  %template() vector< vector< int > >;
  %template() vector< vector< long int> >;
  %template() vector< vector< float > >;
  %template() vector< vector< double > >;

  %template() vector<string>;

  %template(gr_vsize_t) std::vector<size_t>;
  %template(gr_vvvsize_t) std::vector< std::vector< std::vector<size_t> > >;
};

%template(gr_vector_complexf) std::vector< std::complex<float> >;
%template(gr_vector_complexd) std::vector< std::complex<double> >;
%template(gr_vector_vector_complexf) std::vector< std::vector< std::complex<float> > >;
%template(gr_vector_vector_complexd) std::vector< std::vector< std::complex<double> > >;

// Fix for Issue #529: replace std::vector<size_t> with its equivalent
// in element size, one of "unsigned int", "unsigned long", or
// "unsigned long long". The replacement depends on the sizeof each
// type, as determined in GrSwig.cmake GR_SWIG_MAKE. For SWIG >=
// 3.0.0, none of these will be defined because this issue seems to
// have been fixed.

#if defined(SIZE_T_UINT)
  %apply std::vector<unsigned int> { std::vector<size_t> };
#elif defined(SIZE_T_UL)
  %apply std::vector<unsigned long> { std::vector<size_t> };
#elif defined(SIZE_T_ULL)
  %apply std::vector<unsigned long long> { std::vector<size_t> };
#endif

#endif /* SWIG_GR_TYPES_I */
