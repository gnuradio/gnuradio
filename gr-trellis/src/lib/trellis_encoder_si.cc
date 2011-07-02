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

// WARNING: this file is machine generated.  Edits will be over written

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <trellis_encoder_si.h>
#include <gr_io_signature.h>
#include <iostream>

trellis_encoder_si_sptr 
trellis_make_encoder_si (const fsm &FSM, int ST)
{
  return gnuradio::get_initial_sptr (new trellis_encoder_si (FSM,ST));
}

trellis_encoder_si::trellis_encoder_si (const fsm &FSM, int ST)
  : gr_sync_block ("encoder_si",
		   gr_make_io_signature (1, -1, sizeof (short)),
		   gr_make_io_signature (1, -1, sizeof (int))),
    d_FSM (FSM),
    d_ST (ST)
{
}



int 
trellis_encoder_si::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  int ST_tmp=0;

  assert (input_items.size() == output_items.size());
  int nstreams = input_items.size();

  for (int m=0;m<nstreams;m++) {
    const short *in = (const short *) input_items[m];
    int *out = (int *) output_items[m];
    ST_tmp = d_ST;

    // per stream processing
    for (int i = 0; i < noutput_items; i++){
      out[i] = (int) d_FSM.OS()[ST_tmp*d_FSM.I()+in[i]]; // direction of time?
      ST_tmp = (int) d_FSM.NS()[ST_tmp*d_FSM.I()+in[i]];
    }
    // end per stream processing
  }
  d_ST = ST_tmp;

  return noutput_items;
}

