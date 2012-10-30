/* -*- c++ -*- */
/*
 * Copyright 2003,2004,2009 Free Software Foundation, Inc.
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

// Disable warning about base class types
#pragma SWIG nowarn=401

////////////////////////////////////////////////////////////////////////
// gnuradio.i
// SWIG interface definition
////////////////////////////////////////////////////////////////////////

%include <gruel_common.i>

////////////////////////////////////////////////////////////////////////
// Headers

%{
#include "gnuradio_swig_bug_workaround.h"	// mandatory bug fix
#include <gr_types.h>
#include <stddef.h>		// size_t
#include <complex>
%}

%feature("autodoc","1");

// local file
%include <gr_shared_ptr.i>
%include <std_complex.i>
%include <std_vector.i>
%include <stl.i>
%include <std_except.i>

typedef std::complex<float>		gr_complex;
typedef std::complex<double>		gr_complexd;
typedef unsigned long long              uint64_t;
typedef long long                       int64_t;


// instantiate the required template specializations

namespace std {
  %template()	  vector<unsigned char>;
  %template()	  vector<char>;
  %template()	  vector<short>;
  %template()	  vector<int>;
  %template()	  vector<float>;
  %template()	  vector<double>;
  // %template()	  std::complex<float>;

  %template() 	  vector< std::complex<float> >;
  %template()     vector< std::vector< unsigned char > >;
  %template()     vector< std::vector< char > >;
  %template()     vector< std::vector< short > >;
  %template()     vector< std::vector< int > >;
  %template()     vector< std::vector< float > >;
  %template()     vector< std::vector< double > >;
  %template()     vector< std::vector< std::complex<float> > >;
};

////////////////////////////////////////////////////////////////////////

#ifndef SW_RUNTIME
// import runtime.i for all but sw_runtime, since it needs to %include
%import <runtime.i>
#endif

////////////////////////////////////////////////////////////////////////
