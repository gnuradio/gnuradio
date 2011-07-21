/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_WAVELET_FF_H
#define INCLUDED_GR_WAVELET_FF_H

#include <gr_core_api.h>
#include <iostream>
#include <gr_sync_block.h>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_wavelet.h>

class gr_wavelet_ff;
typedef boost::shared_ptr<gr_wavelet_ff> gr_wavelet_ff_sptr;

GR_CORE_API gr_wavelet_ff_sptr
gr_make_wavelet_ff(int size = 1024,
		   int order = 20,
		   bool forward = true);

/*!
 * \brief compute wavelet transform using gsl routines
 * \ingroup wavelet_blk
 */

class GR_CORE_API gr_wavelet_ff : public gr_sync_block
{
  int                    d_size;
  int                    d_order;
  bool			 d_forward;
  gsl_wavelet           *d_wavelet;
  gsl_wavelet_workspace *d_workspace;
  double                *d_temp;

  friend GR_CORE_API gr_wavelet_ff_sptr
    gr_make_wavelet_ff(int size,
		       int order,
		       bool forward);

  gr_wavelet_ff(int size,
		int order,
		bool forward);

public:
  ~gr_wavelet_ff();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_WAVELET_FF_H */
