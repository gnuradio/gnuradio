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
#ifndef INCLUDED_GR_FFT_FILTER_FFF_H
#define INCLUDED_GR_FFT_FILTER_FFF_H

#include <gr_sync_decimator.h>

class gr_fft_filter_fff;
typedef boost::shared_ptr<gr_fft_filter_fff> gr_fft_filter_fff_sptr;
gr_fft_filter_fff_sptr gr_make_fft_filter_fff (int decimation, const std::vector<float> &taps);

class gr_fir_fff;
class gri_fft_real_fwd;
class gri_fft_real_rev;

/*!
 * \brief Fast FFT filter with float input, float output and float taps
 * \ingroup filter
 */
class gr_fft_filter_fff : public gr_sync_decimator
{
 private:
  friend gr_fft_filter_fff_sptr gr_make_fft_filter_fff (int decimation, const std::vector<float> &taps);

  int			   d_ntaps;
  int			   d_nsamples;
  int			   d_fftsize;		// fftsize = ntaps + nsamples - 1
  gri_fft_real_fwd	  *d_fwdfft;		// forward "plan"
  gri_fft_real_rev	  *d_invfft;		// inverse "plan"
  std::vector<float>       d_tail;		// state carried between blocks for overlap-add
  std::vector<gr_complex>  d_xformed_taps;	// Fourier xformed taps
  std::vector<float>	   d_new_taps;
  bool			   d_updated;

  /*!
   * Construct a FFT filter with the given taps
   *
   * \param decimation	>= 1
   * \param taps        float filter taps
   */
  gr_fft_filter_fff (int decimation, const std::vector<float> &taps);

  void compute_sizes(int ntaps);
  int tailsize() const { return d_ntaps - 1; }
  void actual_set_taps (const std::vector<float> &taps);
  
 public:
  ~gr_fft_filter_fff ();

  void set_taps (const std::vector<float> &taps);

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_FFT_FILTER_FFF_H */
