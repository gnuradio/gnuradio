/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FFT_VCC_FFTW_H
#define INCLUDED_GR_FFT_VCC_FFTW_H

#include <gr_core_api.h>
#include <gr_fft_vcc.h>

class gri_fft_complex;

GR_CORE_API gr_fft_vcc_sptr
gr_make_fft_vcc_fftw (int fft_size, bool forward,
		      const std::vector<float> &window,
		      bool shift=false, int nthreads=1);

/*!
 * \brief Compute forward or reverse FFT.  complex vector in / complex vector out.
 * \ingroup dft_blk
 *
 * Concrete class that uses FFTW.
 */
class GR_CORE_API gr_fft_vcc_fftw : public gr_fft_vcc
{
  friend GR_CORE_API gr_fft_vcc_sptr
  gr_make_fft_vcc_fftw (int fft_size, bool forward,
			const std::vector<float> &window,
			bool shift, int nthreads);

  gri_fft_complex *d_fft;

  gr_fft_vcc_fftw (int fft_size, bool forward,
		   const std::vector<float> &window,
		   bool shift, int nthreads=1);

 public:
  ~gr_fft_vcc_fftw ();

  void set_nthreads(int n);
  int nthreads() const;

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};


#endif /* INCLUDED_GR_FFT_VCC_FFTW_H */
