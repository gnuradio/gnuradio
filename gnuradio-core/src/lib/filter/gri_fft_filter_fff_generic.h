/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GRI_FFT_FILTER_FFF_GENERIC_H
#define INCLUDED_GRI_FFT_FILTER_FFF_GENERIC_H

#include <gr_core_api.h>
#include <gr_complex.h>
#include <vector>

class gri_fft_real_fwd;
class gri_fft_real_rev;

class GR_CORE_API gri_fft_filter_fff_generic
{
 private:
  int			   d_ntaps;
  int			   d_nsamples;
  int			   d_fftsize;		// fftsize = ntaps + nsamples - 1
  int                      d_decimation;
  gri_fft_real_fwd	  *d_fwdfft;		// forward "plan"
  gri_fft_real_rev	  *d_invfft;		// inverse "plan"
  int                      d_nthreads;          // number of FFTW threads to use
  std::vector<float>       d_tail;		// state carried between blocks for overlap-add
  std::vector<float>	   d_new_taps;
  gr_complex              *d_xformed_taps;	// Fourier xformed taps


  void compute_sizes(int ntaps);
  int tailsize() const { return d_ntaps - 1; }
  
 public:
  /*!
   * \brief Construct a FFT filter for float vectors with the given taps and decimation rate.
   *
   * This is the basic implementation for performing FFT filter for fast convolution
   * in other blocks for floating point vectors (such as gr_fft_filter_fff).
   * \param decimation The decimation rate of the filter (int)
   * \param taps       The filter taps (float)
   * \param nthreads   The number of threads for the FFT to use (int)
   */
  gri_fft_filter_fff_generic (int decimation, const std::vector<float> &taps,
			      int nthreads=1);
  ~gri_fft_filter_fff_generic ();

  /*!
   * \brief Set new taps for the filter.
   *
   * Sets new taps and resets the class properties to handle different sizes
   * \param taps       The filter taps (float)
   */
  int set_taps (const std::vector<float> &taps);
  
  /*!
   * \brief Set number of threads to use.
   */
  void set_nthreads(int n);

  /*!
   * \brief Get number of threads being used.
   */
  int nthreads() const;
  
  /*!
   * \brief Perform the filter operation
   *
   * \param nitems  The number of items to produce
   * \param input   The input vector to be filtered
   * \param output  The result of the filter operation
   */
  int filter (int nitems, const float *input, float *output);

};

#endif /* INCLUDED_GRI_FFT_FILTER_FFF_GENERIC_H */
