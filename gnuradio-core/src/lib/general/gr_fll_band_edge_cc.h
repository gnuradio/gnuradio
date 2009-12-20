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
 */

class gr_fll_band_edge_cc : public gr_sync_block
{
 private:
  /*!
   * Build the FLL
   * \param taps    (vector/list of gr_complex) The taps of the band-edge filter
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
