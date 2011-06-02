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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_dc_blocker_cc.h>
#include <gr_io_signature.h>
#include <cstdio>

moving_averager_c::moving_averager_c(int D)
  : d_length(D), d_out(0), d_out_d1(0), d_out_d2(0)
{
  d_delay_line = std::deque<gr_complex>(d_length-1, gr_complex(0,0));
}

moving_averager_c::~moving_averager_c()
{
}

gr_complex
moving_averager_c::filter(gr_complex x)
{
  d_out_d1 = d_out;
  d_delay_line.push_back(x);
  d_out = d_delay_line[0];
  d_delay_line.pop_front();
    
  gr_complex y = x - d_out_d1 + d_out_d2;
  d_out_d2 = y;
    
  return (y / (float)(d_length)); 
}



gr_dc_blocker_cc_sptr gr_make_dc_blocker_cc (int D, bool long_form)
{
  return gnuradio::get_initial_sptr(new gr_dc_blocker_cc(D, long_form));
}


gr_dc_blocker_cc::gr_dc_blocker_cc (int D, bool long_form)
  : gr_sync_block ("dc_blocker_cc",
		   gr_make_io_signature (1, 1, sizeof(gr_complex)),
		   gr_make_io_signature (1, 1, sizeof(gr_complex))),
    d_length(D), d_long_form(long_form)
{
  if(d_long_form) {
    d_ma_0 = new moving_averager_c(D);
    d_ma_1 = new moving_averager_c(D);
    d_ma_2 = new moving_averager_c(D);
    d_ma_3 = new moving_averager_c(D);
    d_delay_line = std::deque<gr_complex>(d_length-1, gr_complex(0,0));
  }
  else {
    d_ma_0 = new moving_averager_c(D);
    d_ma_1 = new moving_averager_c(D);
  }
}

gr_dc_blocker_cc::~gr_dc_blocker_cc()
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
gr_dc_blocker_cc::get_group_delay()
{
  if(d_long_form)
    return (2*d_length-2);
  else
    return d_length - 1;
}

int
gr_dc_blocker_cc::work(int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex*)input_items[0];
  gr_complex *out = (gr_complex*)output_items[0];

  if(d_long_form) {
    gr_complex y1, y2, y3, y4, d;
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
    gr_complex y1, y2;
    for(int i = 0; i < noutput_items; i++) {
      y1 = d_ma_0->filter(in[i]);
      y2 = d_ma_1->filter(y1);
      out[i] = d_ma_0->delayed_sig() - y2;
    }
  }

  return noutput_items;
}
