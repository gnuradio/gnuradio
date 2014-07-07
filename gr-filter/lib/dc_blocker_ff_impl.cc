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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dc_blocker_ff_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>

namespace gr {
  namespace filter {

    moving_averager_f::moving_averager_f(int D)
      : d_length(D), d_out(0), d_out_d1(0), d_out_d2(0)
    {
      d_delay_line = std::deque<float>(d_length-1, 0);
    }

    moving_averager_f::~moving_averager_f()
    {
    }

    float
    moving_averager_f::filter(float x)
    {
      d_out_d1 = d_out;
      d_delay_line.push_back(x);
      d_out = d_delay_line[0];
      d_delay_line.pop_front();

      float y = x - d_out_d1 + d_out_d2;
      d_out_d2 = y;

      return (y / (float)(d_length));
    }


    dc_blocker_ff::sptr dc_blocker_ff::make(int D, bool long_form)
    {
      return gnuradio::get_initial_sptr(new dc_blocker_ff_impl(D, long_form));
    }

    dc_blocker_ff_impl::dc_blocker_ff_impl(int D, bool long_form)
      : sync_block("dc_blocker_ff",
		      io_signature::make (1, 1, sizeof(float)),
		      io_signature::make (1, 1, sizeof(float))),
	d_length(D), d_long_form(long_form)
    {
      if(d_long_form) {
	d_ma_0 = new moving_averager_f(D);
	d_ma_1 = new moving_averager_f(D);
	d_ma_2 = new moving_averager_f(D);
	d_ma_3 = new moving_averager_f(D);
	d_delay_line = std::deque<float>(d_length-1, 0);
      }
      else {
	d_ma_0 = new moving_averager_f(D);
	d_ma_1 = new moving_averager_f(D);
        d_ma_2 = NULL;
        d_ma_3 = NULL;
      }
    }

    dc_blocker_ff_impl::~dc_blocker_ff_impl()
    {
      if(d_long_form) {
	delete d_ma_0;
	delete d_ma_1;
	delete d_ma_2;
	delete d_ma_3;
      }
      else {
	delete d_ma_0;
	delete d_ma_1;
      }
    }

    int
    dc_blocker_ff_impl::group_delay()
    {
      if(d_long_form)
	return (2*d_length-2);
      else
	return d_length - 1;
    }

    int
    dc_blocker_ff_impl::work(int noutput_items,
			     gr_vector_const_void_star &input_items,
			     gr_vector_void_star &output_items)
    {
      const float *in = (const float*)input_items[0];
      float *out = (float*)output_items[0];

      if(d_long_form) {
	float y1, y2, y3, y4, d;
	for(int i = 0; i < noutput_items; i++) {
	  y1 = d_ma_0->filter(in[i]);
	  y2 = d_ma_1->filter(y1);
	  y3 = d_ma_2->filter(y2);
	  y4 = d_ma_3->filter(y3);

	  d_delay_line.push_back(d_ma_0->delayed_sig());
	  d = d_delay_line[0];
	  d_delay_line.pop_front();

	  out[i] = d - y4;
	}
      }
      else {
	float y1, y2;
	for(int i = 0; i < noutput_items; i++) {
	  y1 = d_ma_0->filter(in[i]);
	  y2 = d_ma_1->filter(y1);
	  out[i] = d_ma_0->delayed_sig() - y2;
	}
      }

      return noutput_items;
    }

  } /* namespace filter */
} /* namespace gr */
