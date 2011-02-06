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

#ifndef HAVE_CONFIG_H
#include "config.h"
#endif

#include <trellis_constellation_metrics_cf.h>
#include <gr_io_signature.h>
#include <assert.h>
#include <stdexcept>
#include <iostream>



trellis_constellation_metrics_cf_sptr
trellis_make_constellation_metrics_cf (gr_constellation_sptr constellation, trellis_metric_type_t TYPE)
{
  return gnuradio::get_initial_sptr (new trellis_constellation_metrics_cf (constellation, TYPE));
}



trellis_constellation_metrics_cf::trellis_constellation_metrics_cf (gr_constellation_sptr constellation, trellis_metric_type_t TYPE)
  : gr_block ("constellation_metrics_cf",
	      gr_make_io_signature (1, -1, sizeof (gr_complex)),
	      gr_make_io_signature (1, -1, sizeof (float))),
    d_constellation (constellation),
    d_TYPE (TYPE),
    d_O (constellation->arity())
{
  set_relative_rate (d_O);
  set_output_multiple (d_O);
}

void
trellis_constellation_metrics_cf::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  assert (noutput_items % d_O == 0);
  int input_required =  noutput_items / d_O;
  ninput_items_required[0] = input_required;
}



int
trellis_constellation_metrics_cf::general_work (int noutput_items,
				gr_vector_int &ninput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
{

  assert (noutput_items % d_O == 0);

  const gr_complex *in = (gr_complex *) input_items[0];
  float *out = (float *) output_items[0];

  for (int i = 0; i < noutput_items / d_O ; i++){
    d_constellation->calc_metric(in[i], &(out[i*d_O]), d_TYPE); 
  } 

  consume_each (noutput_items / d_O);
  return noutput_items;
}
