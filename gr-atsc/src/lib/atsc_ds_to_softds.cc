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

#include <atsc_ds_to_softds.h>
#include <gr_io_signature.h>
#include <atsc_consts.h>


atsc_ds_to_softds_sptr
atsc_make_ds_to_softds()
{
  return gnuradio::get_initial_sptr(new atsc_ds_to_softds());
}

atsc_ds_to_softds::atsc_ds_to_softds()
  : gr_sync_block("atsc_ds_to_softds",
		  gr_make_io_signature(1, 1, sizeof(atsc_data_segment)),
		  gr_make_io_signature(1, 1, sizeof(atsc_soft_data_segment)))
{
  reset();
}

#define NELEM(x) (sizeof (x) / sizeof (x[0]))

void
atsc_ds_to_softds::map_to_soft_symbols (atsc_soft_data_segment &out,
                     const atsc_data_segment &in)
{
  for (unsigned int i = 0; i < NELEM (in.data); i++){
    out.data[i] = in.data[i] * 2 - 7;
  }
}


int
atsc_ds_to_softds::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  const atsc_data_segment *in = (const atsc_data_segment *) input_items[0];
  atsc_soft_data_segment *out = (atsc_soft_data_segment *) output_items[0];

  for (int i = 0; i < noutput_items; i++){

    out[i].pli = in[i].pli;			// copy pipeline info...
    map_to_soft_symbols(out[i], in[i]);
  }

  return noutput_items;
}
