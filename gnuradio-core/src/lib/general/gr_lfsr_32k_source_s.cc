/* -*- c++ -*- */
/*
 * Copyright 2004,2010 Free Software Foundation, Inc.
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
#include <gr_lfsr_32k_source_s.h>
#include <gr_io_signature.h>
#include <stdexcept>


gr_lfsr_32k_source_s_sptr 
gr_make_lfsr_32k_source_s ()
{
  return gnuradio::get_initial_sptr(new gr_lfsr_32k_source_s ());
}


gr_lfsr_32k_source_s::gr_lfsr_32k_source_s ()
  : gr_sync_block ("lfsr_32k_source_s",
		   gr_make_io_signature (0, 0, 0),
		   gr_make_io_signature (1, 1, sizeof (short))),
    d_index (0)
{
  gri_lfsr_32k	lfsr;
  
  for (int i = 0; i < BUFSIZE; i++)
    d_buffer[i] = lfsr.next_short ();
}

int
gr_lfsr_32k_source_s::work (int noutput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items)
{
  short *out = (short *) output_items[0];
  short *buf = d_buffer;
  int	index = d_index;
  
  for (int i = 0; i < noutput_items; i++){
    out[i] = buf[index];
    // index = (index + 1) & (BUFSIZE - 1);
    index = index + 1;
    if (index >= BUFSIZE)
      index = 0;
  }

  d_index = index;
  return noutput_items;
}
