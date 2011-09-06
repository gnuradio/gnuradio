/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_PLL_CARRIERTRACKING_CC_H
#define INCLUDED_GR_PLL_CARRIERTRACKING_CC_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gri_control_loop.h>

class gr_pll_carriertracking_cc;
typedef boost::shared_ptr<gr_pll_carriertracking_cc> gr_pll_carriertracking_cc_sptr;

GR_CORE_API gr_pll_carriertracking_cc_sptr gr_make_pll_carriertracking_cc (float loop_bw,
							       float max_freq,
							       float min_freq);
/*!
 * \brief Implements a PLL which locks to the input frequency and outputs the 
 * input signal mixed with that carrier.
 * \ingroup sync_blk
 *
 * input: stream of complex; output: stream of complex
 *
 * This PLL locks onto a [possibly noisy] reference carrier on
 * the input and outputs that signal, downconverted to DC
 * 
 * All settings max_freq and min_freq are in terms of radians per sample, 
 * NOT HERTZ.  The loop bandwidth determins the lock range and should be set
 * around pi/200 -- 2pi/100.
 * \sa gr_pll_freqdet_cf, gr_pll_carriertracking_cc
 */

class GR_CORE_API gr_pll_carriertracking_cc : public gr_sync_block, public gri_control_loop
{
  friend GR_CORE_API gr_pll_carriertracking_cc_sptr gr_make_pll_carriertracking_cc (float loop_bw,
									float max_freq,
									float min_freq);

  float d_locksig,d_lock_threshold;
  bool d_squelch_enable;
  gr_pll_carriertracking_cc (float loop_bw, float max_freq, float min_freq);

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
private:
  float mod_2pi (float in);
  float phase_detector(gr_complex sample,float ref_phase);
public:
  bool lock_detector(void);
  bool  squelch_enable(bool);
  float set_lock_threshold(float);
};

#endif
