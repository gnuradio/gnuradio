/* -*- c++ -*- */
/*
 * Copyright 2004,2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FFT_VCC_H
#define INCLUDED_GR_FFT_VCC_H

#include <gr_sync_block.h>

class gri_fft_complex;

class gr_fft_vcc;
typedef boost::shared_ptr<gr_fft_vcc> gr_fft_vcc_sptr;

gr_fft_vcc_sptr
gr_make_fft_vcc (int fft_size, bool forward, const std::vector<float> window, bool shift=false);

/*!
 * \brief Compute forward or reverse FFT.  complex vector in / complex vector out.
 * \ingroup dft
 */

class gr_fft_vcc : public gr_sync_block
{
  friend gr_fft_vcc_sptr
  gr_make_fft_vcc (int fft_size, bool forward, const std::vector<float> window, bool shift);

  unsigned int	   d_fft_size;
  std::vector<float>   d_window;
  gri_fft_complex *d_fft;
  bool d_forward;
  bool d_shift;

  gr_fft_vcc (int fft_size, bool forward, const std::vector<float> window, bool shift);

 public:
  ~gr_fft_vcc ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
  bool set_window(const std::vector<float> window);
};


#endif /* INCLUDED_GR_FFT_VCC_H */
