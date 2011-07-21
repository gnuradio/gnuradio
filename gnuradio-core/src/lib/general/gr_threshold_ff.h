/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_THRESHOLD_FF_H
#define INCLUDED_GR_THRESHOLD_FF_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_threshold_ff;
typedef boost::shared_ptr<gr_threshold_ff> gr_threshold_ff_sptr;

GR_CORE_API gr_threshold_ff_sptr gr_make_threshold_ff (float lo, float hi, float initial_state=0);

/*!
 * \brief Please fix my documentation
 * \ingroup misc_blk
 */
class GR_CORE_API gr_threshold_ff : public gr_sync_block
{
  friend GR_CORE_API gr_threshold_ff_sptr gr_make_threshold_ff (float lo, float hi, float initial_state);

  float	d_lo,d_hi;		// the constant
  float d_last_state;
  gr_threshold_ff (float lo, float hi, float initial_state);

 public:
  float lo () const { return d_lo; }
  void set_lo (float lo) { d_lo = lo; }
  float hi () const { return d_hi; }
  void set_hi (float hi) { d_hi = hi; }
  float last_state () const { return d_last_state; }
  void set_last_state (float last_state) { d_last_state = last_state; }

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif
