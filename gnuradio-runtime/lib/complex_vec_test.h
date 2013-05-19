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

#include <gnuradio/api.h>
#include <vector>
#include <complex>

GR_RUNTIME_API std::vector<std::complex<float> >
complex_vec_test0();

GR_RUNTIME_API std::vector<std::complex<float> >
complex_vec_test1(const std::vector<std::complex<float> > &input);

GR_RUNTIME_API std::complex<float>
complex_scalar_test0();

GR_RUNTIME_API std::complex<float>
complex_scalar_test1(std::complex<float> input);

GR_RUNTIME_API std::vector<int>
int_vec_test0();

GR_RUNTIME_API std::vector<int>
int_vec_test1(const std::vector<int> &input);

GR_RUNTIME_API std::vector<float>
float_vec_test0();

GR_RUNTIME_API std::vector<float>
float_vec_test1(const std::vector<float> &input);

