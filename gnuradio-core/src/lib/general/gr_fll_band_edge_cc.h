/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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


#ifndef INCLUDED_GR_FLL_BAND_EDGE_CC_H
#define	INCLUDED_GR_FLL_BAND_EDGE_CC_H

#include <gr_sync_block.h>

class gr_fll_band_edge_cc;
typedef boost::shared_ptr<gr_fll_band_edge_cc> gr_fll_band_edge_cc_sptr;
gr_fll_band_edge_cc_sptr gr_make_fll_band_edge_cc (float samps_per_sym, float rolloff,
						   int filter_size, float alpha, float beta);

class gr_fir_ccc;
class gri_fft_complex;

/*!
 * \class gr_fll_band_edge_cc
 * \brief Frequency Lock Loop using band-edge filters
 *
 * \ingroup general
 *
 * The frequency lock loop derives a band-edge filter that covers the upper and lower bandwidths
 * of a digitally-modulated signal. The bandwidth range is determined by the excess bandwidth
 * (e.g., rolloff factor) of the modulated signal. The placement in frequency of the band-edges
 * is determined by the oversampling ratio (number of samples per symbol) and the excess bandwidth.
 * The size of the filters should be fairly large so as to average over a number of symbols.
 *
 * The FLL works by filtering the upper and lower band edges into x_u(t) and x_l(t), respectively.
 * These are combined to form cc(t) = x_u(t) + x_l(t) and ss(t) = x_u(t) - x_l(t). Combining
 * these to form the signal e(t) = Re{cc(t) \times ss(t)^*} (where ^* is the complex conjugate)
 * provides an error signal at the DC term that is directly proportional to the carrier frequency.
 * We then make a second-order loop using the error signal that is the running average of e(t).
 *
 * In theory, the band-edge filter is the derivative of the matched filter in frequency, 
 * (H_be(f) = \frac{H(f)}{df}. In practice, this comes down to a quarter sine wave at the point
 * of the matched filter's rolloff (if it's a raised-cosine, the derivative of a cosine is a sine).
 * Extend this sine by another quarter wave to make a half wave around the band-edges is equivalent
 * in time to the sum of two sinc functions. The baseband filter fot the band edges is therefore
 * derived from this sum of sincs. The band edge filters are then just the baseband signal
 * modulated to the correct place in frequency. All of these calculations are done in the
 * 'design_filter' function.
 *
 * Note: We use FIR filters here because the filters have to have a flat phase response over the
 * entire frequency range to allow their comparisons to be valid.
 */

class gr_fll_band_edge_cc : public gr_sync_block
{
 private:
  /*!
   * Build the FLL
   * \param samps_per_sym    (float) Number of samples per symbol of signal
   * \param rolloff          (float) Rolloff factor of signal
   * \param filter_size      (int)   Size (in taps) of the filter
   * \param alpha            (float) Loop gain 1
   * \param beta             (float) Loop gain 2
   */
  friend gr_fll_band_edge_cc_sptr gr_make_fll_band_edge_cc (float samps_per_sym, float rolloff,
							    int filter_size, float alpha, float beta);

  float                   d_alpha;
  float                   d_beta;
  float                   d_max_freq;
  float                   d_min_freq;

  gr_fir_ccc*             d_filter_upper;
  gr_fir_ccc*             d_filter_lower;
  bool			  d_updated;
  float                   d_error;
  float                   d_freq;
  float                   d_phase;

  /*!
   * Build the FLL
   * \param taps    (vector/list of gr_complex) The taps of the band-edge filter
   */
  gr_fll_band_edge_cc(float samps_per_sym, float rolloff,
		      int filter_size, float alpha, float beta);

public:
  ~gr_fll_band_edge_cc ();
  
  /*!
   * Design the band-edge filter based on the number of samples per symbol,
   * filter rolloff factor, and the filter size
   * \param samps_per_sym    (float) Number of samples per symbol of signal
   * \param rolloff          (float) Rolloff factor of signal
   * \param filter_size      (int)   Size (in taps) of the filter
   */
  void design_filter(float samps_per_sym, float rolloff, int filter_size);

  /*!
   * Set the alpha gainvalue
   * \param alpha    (float) new gain value
   */
  void set_alpha(float alpha);

  /*!
   * Set the beta gain value
   * \param beta    (float) new gain value
   */
  void set_beta(float beta) { d_beta = beta; }

  /*!
   * Print the taps to screen.
   */
  void print_taps();
   
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif
