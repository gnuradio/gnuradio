/* -*- c++ -*- */
/*
 * Copyright 2003,2004,2005,2010 Free Software Foundation, Inc.
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

#include <gr_oscope_sink_f.h>
#include <gr_io_signature.h>
#include <gr_oscope_guts.h>


gr_oscope_sink_f_sptr
gr_make_oscope_sink_f (double sampling_rate, gr_msg_queue_sptr msgq)
{
  return gnuradio::get_initial_sptr(new gr_oscope_sink_f (sampling_rate, msgq));
}


gr_oscope_sink_f::gr_oscope_sink_f (double sampling_rate, gr_msg_queue_sptr msgq)
  : gr_oscope_sink_x ("oscope_sink_f",
		      gr_make_io_signature (1, gr_oscope_guts::MAX_CHANNELS, sizeof (float)),
		      sampling_rate),
    d_msgq(msgq)
{
  d_guts = new gr_oscope_guts (d_sampling_rate, d_msgq);
}


bool
gr_oscope_sink_f::check_topology (int ninputs, int noutputs)
{
  return d_guts->set_num_channels(ninputs);
}


gr_oscope_sink_f::~gr_oscope_sink_f ()
{
}

int
gr_oscope_sink_f::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  int	  ni = input_items.size ();
  float	  tmp[gr_oscope_guts::MAX_CHANNELS];

  for (int i = 0; i < noutput_items; i++){

    // FIXME for now, copy the data.  Fix later if reqd
    for (int ch = 0; ch < ni; ch++)	
      tmp[ch] = ((const float *) input_items[ch])[i];

    d_guts->process_sample (tmp);
  }

  return noutput_items;
}
