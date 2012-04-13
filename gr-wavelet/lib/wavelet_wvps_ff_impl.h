/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_WAVELET_WVPS_FF_IMPL_H
#define INCLUDED_WAVELET_WVPS_FF_IMPL_H

#include <wavelet_wvps_ff.h>

class WAVELET_API wavelet_wvps_ff_impl : public wavelet_wvps_ff
{
  int d_ilen;
  int d_olen;

  friend WAVELET_API wavelet_wvps_ff_sptr
  wavelet_make_wvps_ff(int ilen);

  wavelet_wvps_ff_impl(int ilen);

 public:
  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_WAVELET_WVPS_FF_IMPL_H */
