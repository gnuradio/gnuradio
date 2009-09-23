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

#include <noaa_hrpt_decoder.h>
#include <gr_io_signature.h>

#define SYNC1 0x0284
#define SYNC2 0x016F
#define SYNC3 0x035C
#define SYNC4 0x019D
#define SYNC5 0x020F
#define SYNC6 0x0095

noaa_hrpt_decoder_sptr
noaa_make_hrpt_decoder()
{
  return gnuradio::get_initial_sptr(new noaa_hrpt_decoder());
}

noaa_hrpt_decoder::noaa_hrpt_decoder()
  : gr_sync_block("noaa_hrpt_decoder",
		  gr_make_io_signature(1, 1, sizeof(short)),
		  gr_make_io_signature(0, 0, 0))
{
  d_word_count = 0;
}

int
noaa_hrpt_decoder::work(int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  const unsigned short *in = (const unsigned short*)input_items[0];

  int i = 0;
  while (i < noutput_items) {
    unsigned short word = in[i++];
    d_word_count++;
    //fprintf(stderr, "%5u:  ", d_word_count);
    for (int pos = 0; pos < 10; pos++) {
      char ch = (word & (1 << 9)) ? '1' : '0';
      word = word << 1;
      //fprintf(stderr, "%c ", ch);
    }
    //fprintf(stderr, "\n");

    if (d_word_count == 11090) {
      d_word_count = 0;
      //fprintf(stderr, "\n");
    }
  }

  return i;
}
