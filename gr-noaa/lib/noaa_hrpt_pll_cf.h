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

#ifndef INCLUDED_NOAA_HRPT_PLL_CF_H
#define INCLUDED_NOAA_HRPT_PLL_CF_H

#include <noaa_api.h>
#include <gr_sync_block.h>

class noaa_hrpt_pll_cf;
typedef boost::shared_ptr<noaa_hrpt_pll_cf> noaa_hrpt_pll_cf_sptr;

NOAA_API noaa_hrpt_pll_cf_sptr
noaa_make_hrpt_pll_cf(float alpha, float beta, float max_offset);

class NOAA_API noaa_hrpt_pll_cf : public gr_sync_block
{
  friend NOAA_API noaa_hrpt_pll_cf_sptr noaa_make_hrpt_pll_cf(float alpha, float beta, float max_offset);
  noaa_hrpt_pll_cf(float alpha, float beta, float max_offset);

  float d_alpha;		// 1st order loop constant
  float d_beta;			// 2nd order loop constant
  float d_max_offset;		// Maximum frequency offset, radians/sample
  float d_phase;		// Instantaneous carrier phase
  float d_freq;			// Instantaneous carrier frequency, radians/sample

 public:
  virtual int work(int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items);

  void set_alpha(float alpha) { d_alpha = alpha; }
  void set_beta(float beta) { d_beta = beta; }
  void set_max_offset(float max_offset) { d_max_offset = max_offset; }
};

#endif /* INCLUDED_NOAA_HRPT_PLL_CF_H */
