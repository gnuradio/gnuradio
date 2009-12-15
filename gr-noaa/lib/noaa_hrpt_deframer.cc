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

#include <noaa_hrpt_deframer.h>
#include <gr_io_signature.h>
#include <noaa_hrpt.h>
#include <cstring>
#include <cstdio>

#define ST_IDLE   0
#define ST_SYNCED 1

noaa_hrpt_deframer_sptr
noaa_make_hrpt_deframer()
{
  return gnuradio::get_initial_sptr(new noaa_hrpt_deframer());
}

noaa_hrpt_deframer::noaa_hrpt_deframer()
  : gr_block("noaa_hrpt_deframer",
	     gr_make_io_signature(1, 1, sizeof(char)),
	     gr_make_io_signature(1, 1, sizeof(short)))
{
  set_output_multiple(6); // room for writing full sync when received
  d_mid_bit = true;
  d_last_bit = 0;
  enter_idle();
}

void
noaa_hrpt_deframer::enter_idle()
{
  d_state = ST_IDLE;
}

void
noaa_hrpt_deframer::enter_synced()
{
  d_state = ST_SYNCED;
  d_bit_count = HRPT_BITS_PER_WORD;
  d_word_count = HRPT_MINOR_FRAME_WORDS-HRPT_SYNC_WORDS;
  d_word = 0;
}

int
noaa_hrpt_deframer::general_work(int noutput_items,
				 gr_vector_int &ninput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
{
  int ninputs = ninput_items[0];
  const char *in = (const char *)input_items[0];
  unsigned short *out = (unsigned short *)output_items[0];

  int i = 0, j = 0;
  while (i < ninputs && j < noutput_items) {
    char bit = in[i++];
    char diff = bit^d_last_bit;
    d_last_bit = bit;

    // Wait for transition if not synced, otherwise, alternate bits
    if (d_mid_bit && (diff | (d_state == ST_SYNCED))) {
      switch (d_state) {
      case ST_IDLE:
	d_shifter = (d_shifter << 1) | bit; // MSB transmitted first
	
	if ((d_shifter & 0x0FFFFFFFFFFFFFFFLL) == HRPT_MINOR_FRAME_SYNC) {
	  out[j++] = HRPT_SYNC1;
	  out[j++] = HRPT_SYNC2;
	  out[j++] = HRPT_SYNC3;
	  out[j++] = HRPT_SYNC4;
	  out[j++] = HRPT_SYNC5;
	  out[j++] = HRPT_SYNC6;
	  enter_synced();
	}
	break;
	
      case ST_SYNCED:
	d_word = (d_word << 1) | bit; // MSB transmitted first
	if (--d_bit_count == 0) {
	  out[j++] = d_word;
	  d_word = 0;
	  d_bit_count = HRPT_BITS_PER_WORD;
	  if (--d_word_count == 0) {
	    enter_idle();
	  }
	}
	break;
	
      default:
	throw std::runtime_error("noaa_hrpt_deframer: bad state\n");
      }

      d_mid_bit = false;
    }
    else {
      d_mid_bit = true;
    }
  }

  consume_each(i);
  return j;
}
