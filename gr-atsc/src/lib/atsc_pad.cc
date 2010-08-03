/* -*- c++ -*- */
/*
 * Copyright 2006,2010 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <atsc_pad.h>
#include <gr_io_signature.h>
#include <atsc_types.h>

static const int INTR = ATSC_MPEG_PKT_LENGTH;

atsc_pad_sptr
atsc_make_pad()
{
  return gnuradio::get_initial_sptr(new atsc_pad());
}

atsc_pad::atsc_pad()
  : gr_sync_decimator("atsc_pad",
		  gr_make_io_signature(1, 1, sizeof(unsigned char)),
		  gr_make_io_signature(1, 1, sizeof(atsc_mpeg_packet)),
		  INTR)
{
  reset();
}

void
atsc_pad::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned ninputs = ninput_items_required.size();
  for (unsigned i = 0; i < ninputs; i++) 
    ninput_items_required[i] = noutput_items * ATSC_MPEG_PKT_LENGTH;
}


int
atsc_pad::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  const unsigned char *in = (const unsigned char *) input_items[0];
  atsc_mpeg_packet *out = (atsc_mpeg_packet *) output_items[0];
  
  int i;

  for (i = 0; i < noutput_items; i++){
    for (int j = 0; j < ATSC_MPEG_PKT_LENGTH; j++)
	out[i].data[j] = in[i * ATSC_MPEG_PKT_LENGTH + j];

  }

  return noutput_items;
}




