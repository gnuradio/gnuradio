/* -*- c++ -*- */
/*
 * Copyright 2004,2018 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_COMPLEX_H
#define INCLUDED_GR_COMPLEX_H

#include <complex>
typedef std::complex<float>			gr_complex;
typedef std::complex<double>			gr_complexd;

inline bool is_complex (gr_complex x) { (void) x; return true;}
inline bool is_complex (gr_complexd x) { (void) x; return true;}
inline bool is_complex (float x) { (void) x; return false;}
inline bool is_complex (double x) { (void) x; return false;}
inline bool is_complex (int x) { (void) x; return false;}
inline bool is_complex (char x) { (void) x; return false;}
inline bool is_complex (short x) { (void) x; return false;}

#endif /* INCLUDED_GR_COMPLEX_H */

