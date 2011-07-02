/* -*- c++ -*- */
/*
 * Copyright 2008,2010 Free Software Foundation, Inc.
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

#include <gr_integrate_ii.h>
#include <gr_io_signature.h>

gr_integrate_ii_sptr 
gr_make_integrate_ii (int decim)
{
  return gnuradio::get_initial_sptr (new gr_integrate_ii (decim));
}

gr_integrate_ii::gr_integrate_ii (int decim)
  : gr_sync_decimator ("integrate_ii",
		       gr_make_io_signature (1, 1, sizeof (int)),
		       gr_make_io_signature (1, 1, sizeof (int)),
		       decim),
    d_decim(decim),
    d_count(0)
{
}

gr_integrate_ii::~gr_integrate_ii ()
{
}

int 
gr_integrate_ii::work (int noutput_items,
	      gr_vector_const_void_star &input_items,
	      gr_vector_void_star &output_items)
{
  const int *in = (const int *) input_items[0];
  int *out = (int *) output_items[0];
  
  for (int i = 0; i < noutput_items; i++) {
    out[i] = (int)0;
    for (int j = 0; j < d_decim; j++)
      out[i] += in[i*d_decim+j];
  }

  return noutput_items;
}
