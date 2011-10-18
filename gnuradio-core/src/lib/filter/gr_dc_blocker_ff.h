/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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


#ifndef INCLUDED_GR_DC_BLOCKER_FF_H
#define	INCLUDED_GR_DC_BLOCKER_FF_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <deque>

class GR_CORE_API moving_averager_f
{
public:
  moving_averager_f(int D);
  ~moving_averager_f();

  float filter(float x);
  float delayed_sig() { return d_out; }

private:
  int d_length;
  float d_out, d_out_d1, d_out_d2;
  std::deque<float> d_delay_line;
};


class gr_dc_blocker_ff;
typedef boost::shared_ptr<gr_dc_blocker_ff> gr_dc_blocker_ff_sptr;
GR_CORE_API gr_dc_blocker_ff_sptr gr_make_dc_blocker_ff (int D=32, bool long_form=true);

/*!
 * \class gr_dc_blocker_ff
 * \brief a computationally efficient controllable DC blocker
 *
 * \ingroup filter_blk
 * 
 * This block implements a computationally efficient DC blocker that produces
 * a tighter notch filter around DC for a smaller group delay than an
 * equivalent FIR filter or using a single pole IIR filter (though the IIR
 * filter is computationally cheaper).
 *
 * The block defaults to using a delay line of length 32 and the long form
 * of the filter. Optionally, the delay line length can be changed to alter
 * the width of the DC notch (longer lines will decrease the width).
 *
 * The long form of the filter produces a nearly flat response outside of
 * the notch but at the cost of a group delay of 2D-2.
 *
 * The short form of the filter does not have as flat a response in the
 * passband but has a group delay of only D-1 and is cheaper to compute.
 *
 * The theory behind this block can be found in the paper:
 *
 *    <B><EM>R. Yates, "DC Blocker Algorithms," IEEE Signal Processing Magazine,
 *        Mar. 2008, pp 132-134.</EM></B>
 */
class GR_CORE_API gr_dc_blocker_ff : public gr_sync_block
{
 private:
  /*!
   * Build the DC blocker.
   * \param D          (int) the length of the delay line
   * \param long_form  (bool) whether to use long (true, default) or short form
   */
  friend GR_CORE_API gr_dc_blocker_ff_sptr gr_make_dc_blocker_ff (int D, bool long_form);

  int d_length;
  bool d_long_form;
  moving_averager_f *d_ma_0;
  moving_averager_f *d_ma_1;
  moving_averager_f *d_ma_2;
  moving_averager_f *d_ma_3;
  std::deque<float> d_delay_line;

  gr_dc_blocker_ff (int D, bool long_form);

public:
  ~gr_dc_blocker_ff ();

  /*!
   * Get the blocker's group delay that is based on length of delay lines
   */
  int get_group_delay();

  //int set_length(int D);
  
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif
