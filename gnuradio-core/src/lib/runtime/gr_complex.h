/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef INCLUDED_GR_COMPLEX_H
#define INCLUDED_GR_COMPLEX_H

#include <complex>
typedef std::complex<float>			gr_complex;
typedef std::complex<double>			gr_complexd;


inline bool is_complex (gr_complex x) { return true;}
inline bool is_complex (gr_complexd x) { return true;}
inline bool is_complex (float x) { return false;}
inline bool is_complex (double x) { return false;}
inline bool is_complex (int x) { return false;}
inline bool is_complex (char x) { return false;}
inline bool is_complex (short x) { return false;}


// this doesn't really belong here, but there are worse places for it...

#define ASSERT_COMPLEXES_EQUAL(expected,actual,delta)			\
  CPPUNIT_ASSERT_DOUBLES_EQUAL (expected.real(), actual.real(), delta);	\
  CPPUNIT_ASSERT_DOUBLES_EQUAL (expected.imag(), actual.imag(), delta);

#endif /* INCLUDED_GR_COMPLEX_H */

