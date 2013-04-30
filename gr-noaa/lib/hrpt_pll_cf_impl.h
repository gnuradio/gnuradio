/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_NOAA_HRPT_PLL_CF_IMPL_H
#define INCLUDED_NOAA_HRPT_PLL_CF_IMPL_H

#include <gnuradio/noaa/hrpt_pll_cf.h>

namespace gr {
  namespace noaa {

    class hrpt_pll_cf_impl : public hrpt_pll_cf
    {
    private:
      float d_alpha;		// 1st order loop constant
      float d_beta;			// 2nd order loop constant
      float d_max_offset;		// Maximum frequency offset, radians/sample
      float d_phase;		// Instantaneous carrier phase
      float d_freq;			// Instantaneous carrier frequency, radians/sample

    public:
      hrpt_pll_cf_impl(float alpha, float beta, float max_offset);
      ~hrpt_pll_cf_impl();

      virtual int work(int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);

      void set_alpha(float alpha) { d_alpha = alpha; }
      void set_beta(float beta) { d_beta = beta; }
      void set_max_offset(float max_offset) { d_max_offset = max_offset; }
    };

  } /* namespace noaa */
} /* namespace gr */

#endif /* INCLUDED_NOAA_HRPT_PLL_CF_IMPL_H */
