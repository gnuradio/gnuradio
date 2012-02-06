/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FFT_VFC_H
#define INCLUDED_GR_FFT_VFC_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gri_fft_complex;

class gr_fft_vfc;
typedef boost::shared_ptr<gr_fft_vfc> gr_fft_vfc_sptr;

GR_CORE_API gr_fft_vfc_sptr
gr_make_fft_vfc (int fft_size, bool forward,
		 const std::vector<float> &window,
		 int nthreads=1);

/*!
 * \brief Compute forward FFT.  float vector in / complex vector out.
 * \ingroup dft_blk
 */

class GR_CORE_API gr_fft_vfc : public gr_sync_block
{
  friend GR_CORE_API gr_fft_vfc_sptr
  gr_make_fft_vfc (int fft_size, bool forward,
		   const std::vector<float>  &window,
		   int nthreads);

  unsigned int  d_fft_size;
  std::vector<float> d_window;
  gri_fft_complex *d_fft;

  gr_fft_vfc (int fft_size, bool forward,
	      const std::vector<float>  &window,
	      int nthreads=1);

 public:
  ~gr_fft_vfc ();

  void set_nthreads(int n);
  int nthreads() const;

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
  bool set_window(const std::vector<float> &window);
};


#endif /* INCLUDED_GR_FFT_VFC_H */
