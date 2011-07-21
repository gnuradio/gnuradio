/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_DPLL_BB_H
#define INCLUDED_GR_DPLL_BB_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_dpll_bb;
typedef boost::shared_ptr<gr_dpll_bb> gr_dpll_bb_sptr;

GR_CORE_API gr_dpll_bb_sptr gr_make_dpll_bb (float period, float gain);

/*!
 * \brief Detect the peak of a signal
 * \ingroup level_blk
 *
 * If a peak is detected, this block outputs a 1, 
 * or it outputs 0's.
 */
class GR_CORE_API gr_dpll_bb : public gr_sync_block
{
  friend GR_CORE_API gr_dpll_bb_sptr gr_make_dpll_bb (float period, float gain);

  gr_dpll_bb (float period, float gain);

 private:
  unsigned char d_restart;
  float d_pulse_phase, d_pulse_frequency,d_gain,d_decision_threshold;

 public:

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif
