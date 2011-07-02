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

#include <trellis_sccc_encoder_si.h>
#include <gr_io_signature.h>
#include <iostream>

trellis_sccc_encoder_si_sptr 
trellis_make_sccc_encoder_si (
  const fsm &FSMo, int STo,
  const fsm &FSMi, int STi,
  const interleaver &INTERLEAVER,
  int blocklength
)
{
  return gnuradio::get_initial_sptr (new trellis_sccc_encoder_si (FSMo,STo,FSMi,STi,INTERLEAVER,blocklength));
}

trellis_sccc_encoder_si::trellis_sccc_encoder_si (
  const fsm &FSMo, int STo,
  const fsm &FSMi, int STi,
  const interleaver &INTERLEAVER,
  int blocklength
)
  : gr_sync_block ("sccc_encoder_si",
		   gr_make_io_signature (1, 1, sizeof (short)),
		   gr_make_io_signature (1, 1, sizeof (int))),
    d_FSMo (FSMo),
    d_STo (STo),
    d_FSMi (FSMi),
    d_STi (STi),
    d_INTERLEAVER (INTERLEAVER),
    d_blocklength (blocklength)
{
  assert(d_FSMo.O() == d_FSMi.I());
  set_output_multiple(d_blocklength);
  d_buffer.resize(d_blocklength);
}



int 
trellis_sccc_encoder_si::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  assert(noutput_items%d_blocklength ==0);
  for (int b = 0 ; b<noutput_items/d_blocklength; b++) { 
    const short *in = (const short *) input_items[0]+b*d_blocklength;
    int *out = (int *) output_items[0]+b*d_blocklength;

    int STo_tmp = d_STo;
    for (int i = 0; i < d_blocklength; i++){
      d_buffer[i] = d_FSMo.OS()[STo_tmp*d_FSMo.I()+in[i]];
      STo_tmp = (int) d_FSMo.NS()[STo_tmp*d_FSMo.I()+in[i]];
    }
    int STi_tmp = d_STi;
    for (int i = 0; i < d_blocklength; i++){
      int k = d_INTERLEAVER.INTER()[i];
      out[i] = (int) d_FSMi.OS()[STi_tmp*d_FSMi.I()+d_buffer[k]];
      STi_tmp = (int) d_FSMi.NS()[STi_tmp*d_FSMi.I()+d_buffer[k]];
    }
  }
  return noutput_items;
}

