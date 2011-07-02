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

// WARNING: this file is machine generated.  Edits will be over written

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_sample_and_hold_ii.h>
#include <gr_io_signature.h>

gr_sample_and_hold_ii_sptr
gr_make_sample_and_hold_ii ()
{
  return gnuradio::get_initial_sptr (new gr_sample_and_hold_ii ());
}

gr_sample_and_hold_ii::gr_sample_and_hold_ii ()
  : gr_sync_block ("sample_and_hold_ii",
		   gr_make_io_signature2 (2, 2, sizeof (int), sizeof(char)),
		   gr_make_io_signature (1, 1, sizeof (int))),
    d_data(0)
{
}

int
gr_sample_and_hold_ii::work (int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
{
  int *iptr = (int *) input_items[0];
  const char *ctrl = (const char *) input_items[1];
  int *optr = (int *) output_items[0];

  for (int i = 0; i < noutput_items; i++){
    if(ctrl[i]) {
      d_data = iptr[i];
    }
    optr[i] = d_data;
  }
  return noutput_items;
}
