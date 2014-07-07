/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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


#ifndef INCLUDED_FILTER_DC_BLOCKER_FF_IMPL_H
#define	INCLUDED_FILTER_DC_BLOCKER_FF_IMPL_H

#include <gnuradio/filter/dc_blocker_ff.h>
#include <deque>

namespace gr {
  namespace filter {

    class moving_averager_f
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

    class FILTER_API dc_blocker_ff_impl : public dc_blocker_ff
    {
    private:
      int d_length;
      bool d_long_form;
      moving_averager_f *d_ma_0;
      moving_averager_f *d_ma_1;
      moving_averager_f *d_ma_2;
      moving_averager_f *d_ma_3;
      std::deque<float>  d_delay_line;

    public:
      dc_blocker_ff_impl(int D, bool long_form);

      ~dc_blocker_ff_impl();

      int group_delay();

      //int set_length(int D);

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_DC_BLOCKER_FF_IMPL_H */
