/* -*- c++ -*- */
/*
 * Copyright 2003,2004 Free Software Foundation, Inc.
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

////////////////////////////////////////////////////////////////////////
// gnuradio.i
// SWIG interface definition
////////////////////////////////////////////////////////////////////////


#ifndef SWIGIMPORTED
%module(directors="1") gnuradio_swig_py_runtime
#endif

////////////////////////////////////////////////////////////////////////
// Headers


%{
#include <gr_types.h>
#include <stddef.h>		// size_t
%}

%feature("autodoc","1");

%include <shared_ptr.i>
%include <stl.i>
%include <std_complex.i>
%include <std_except.i>

typedef std::complex<float>		gr_complex;
typedef std::complex<double>		gr_complexd;


// instantiate the required template specializations

namespace std {
  %template()	  vector<unsigned char>;
  %template()	  vector<char>;
  %template()	  vector<short>;
  %template()	  vector<int>;
  %template()	  vector<float>;
  %template()	  vector<double>;

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

%constant int sizeof_char 	= sizeof(char);
%constant int sizeof_short	= sizeof(short);
%constant int sizeof_int	= sizeof(int);
%constant int sizeof_float	= sizeof(float);
%constant int sizeof_double	= sizeof(double);
%constant int sizeof_gr_complex	= sizeof(gr_complex);

////////////////////////////////////////////////////////////////////////

%include <runtime.i>
 // %include <general.i>
 // %include <filter.i>
 // %include <io.i>

// %include <atsc.i>

////////////////////////////////////////////////////////////////////////
