/* -*- c++ -*- */
/*
 * Copyright 2007,2010 Free Software Foundation, Inc.
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

#include <gr_regenerate_bb.h>
#include <gr_io_signature.h>

gr_regenerate_bb_sptr
gr_make_regenerate_bb (int period, unsigned int max_regen)
{
  return gnuradio::get_initial_sptr(new gr_regenerate_bb (period, max_regen));
}

gr_regenerate_bb::gr_regenerate_bb (int period, unsigned int max_regen)
  : gr_sync_block ("regenerate_bb",
		   gr_make_io_signature (1, 1, sizeof (char)),
		   gr_make_io_signature (1, 1, sizeof (char))),
    d_period(period),
    d_countdown(0),
    d_max_regen(max_regen),
    d_regen_count(max_regen)
{
}

void gr_regenerate_bb::set_max_regen(unsigned int regen)
{
  d_max_regen = regen;
  d_countdown = 0;
  d_regen_count = d_max_regen;
}

void gr_regenerate_bb::set_period(int period)
{
  d_period = period;
  d_countdown = 0;
  d_regen_count = d_max_regen;
}

int
gr_regenerate_bb::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  const char *iptr = (const char *) input_items[0];
  char *optr = (char *) output_items[0];

  for (int i = 0; i < noutput_items; i++){
    optr[i] = 0;

    if(d_regen_count < d_max_regen) {
      d_countdown--;
      
      if(d_countdown == 0) {
	optr[i] = 1;
	d_countdown = d_period;
	d_regen_count++;
      }
    }
    
    if(iptr[i] == 1) {
      d_countdown = d_period;
      optr[i] = 1;
      d_regen_count = 0;
    }

  }
  return noutput_items;
}
