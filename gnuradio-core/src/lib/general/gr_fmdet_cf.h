/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FMDET_CF_H
#define INCLUDED_GR_FMDET_CF_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_fmdet_cf;
typedef boost::shared_ptr<gr_fmdet_cf> gr_fmdet_cf_sptr;

GR_CORE_API gr_fmdet_cf_sptr gr_make_fmdet_cf (float samplerate, float freq_low, float freq_high, float scl);

class gr_fir_ccf;


/*!
 * \brief Implements an IQ slope detector
 * 
 *
 * input: stream of complex; output: stream of floats
 *
 * This implements a limiting slope detector.  The limiter is in the
 * normalization by the magnitude of the sample 
 */

class GR_CORE_API gr_fmdet_cf : public gr_sync_block
{
  friend GR_CORE_API gr_fmdet_cf_sptr gr_make_fmdet_cf (float samplerate, float freq_low,
					    float freq_high, float scl);

  gr_complex d_S1,d_S2,d_S3,d_S4;
  float d_freq,d_freqlo,d_freqhi,d_scl,d_bias;
  gr_fir_ccf* d_filter;
  gr_fmdet_cf (float samplerate, float freq_low, float freq_high, float scl);

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

};

#endif
