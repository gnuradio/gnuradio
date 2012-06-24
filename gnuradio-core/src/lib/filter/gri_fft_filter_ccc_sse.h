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

#ifndef INCLUDED_GRI_FFT_FILTER_CCC_SSE_H
#define INCLUDED_GRI_FFT_FILTER_CCC_SSE_H

#include <gr_core_api.h>
#include <gr_complex.h>
#include <vector>

class gri_fft_complex;

/*!
 * \brief Fast FFT filter with gr_complex input, gr_complex output and gr_complex taps
 * \ingroup filter_blk
 */
class GR_CORE_API gri_fft_filter_ccc_sse
{
 private:
  int			   d_ntaps;
  int			   d_nsamples;
  int			   d_fftsize;		// fftsize = ntaps + nsamples - 1
  int                      d_decimation;
  gri_fft_complex	  *d_fwdfft;		// forward "plan"
  gri_fft_complex	  *d_invfft;		// inverse "plan"
  std::vector<gr_complex>  d_tail;		// state carried between blocks for overlap-add
  gr_complex              *d_xformed_taps;
  std::vector<gr_complex>  d_new_taps;

  void compute_sizes(int ntaps);
  int tailsize() const { return d_ntaps - 1; }

 public:
  /*!
   * \brief Construct an FFT filter for complex vectors with the given taps and decimation rate.
   *
   * This is the basic implementation for performing FFT filter for fast convolution
   * in other blocks for complex vectors (such as gr_fft_filter_ccc).
   * \param decimation The decimation rate of the filter (int)
   * \param taps       The filter taps (complex)
   */
  gri_fft_filter_ccc_sse (int decimation, const std::vector<gr_complex> &taps);
  ~gri_fft_filter_ccc_sse ();

  /*!
   * \brief Set new taps for the filter.
   *
   * Sets new taps and resets the class properties to handle different sizes
   * \param taps       The filter taps (complex)
   */
  int set_taps (const std::vector<gr_complex> &taps);

  /*!
   * \brief Perform the filter operation
   *
   * \param nitems  The number of items to produce
   * \param input   The input vector to be filtered
   * \param output  The result of the filter operation
   */
  int filter (int nitems, const gr_complex *input, gr_complex *output);

};

#endif /* INCLUDED_GRI_FFT_FILTER_CCC_SSE_H */
