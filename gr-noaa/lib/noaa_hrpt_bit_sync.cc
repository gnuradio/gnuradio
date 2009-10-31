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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <noaa_hrpt_bit_sync.h>
#include <gr_io_signature.h>
#include <iostream>

noaa_hrpt_bit_sync_sptr
noaa_make_hrpt_bit_sync()
{
  return gnuradio::get_initial_sptr(new noaa_hrpt_bit_sync());
}

noaa_hrpt_bit_sync::noaa_hrpt_bit_sync()
  : gr_block("noaa_hrpt_bit_sync",
	     gr_make_io_signature(1, 1, sizeof(char)),
	     gr_make_io_signature(1, 1, sizeof(char))),
    d_mid_bit(true),
    d_last_bit(0)
{
}

int
noaa_hrpt_bit_sync::general_work(int noutput_items,
				 gr_vector_int &ninput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
{
  int ninputs = ninput_items[0];
  const char *in = (const char *)input_items[0];
  char *out = (char *)output_items[0];

  int i = 0, j = 0;
  while (i < ninputs && j < noutput_items) {
    char bit = in[i++];
    char diff = bit^d_last_bit;
    d_last_bit = bit;

    if (d_mid_bit && diff) {
      out[j++] = bit;
      d_mid_bit = false;
    }
    else
      d_mid_bit = true;
  }

  consume_each(i);
  return j;
}
