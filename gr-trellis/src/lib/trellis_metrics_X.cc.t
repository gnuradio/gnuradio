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

// @WARNING@

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <@NAME@.h>
#include <gr_io_signature.h>
#include <assert.h>
#include <stdexcept>
#include <iostream>


@SPTR_NAME@
trellis_make_@BASE_NAME@ (int O, int D,  const std::vector<@I_TYPE@> &TABLE, trellis_metric_type_t TYPE)
{
  return gnuradio::get_initial_sptr (new @NAME@ (O,D,TABLE,TYPE));
}



@NAME@::@NAME@ (int O, int D,  const std::vector<@I_TYPE@> &TABLE, trellis_metric_type_t TYPE)
  : gr_block ("@BASE_NAME@",
	      gr_make_io_signature (1, -1, sizeof (@I_TYPE@)),
	      gr_make_io_signature (1, -1, sizeof (float))),
    d_O (O),
    d_D (D),
    d_TYPE (TYPE),
    d_TABLE (TABLE)
{
  set_relative_rate (1.0 * d_O / ((double) d_D));
  set_output_multiple ((int)d_O);
}

void @NAME@::set_TABLE (const std::vector<@I_TYPE@> &table)
{
  d_TABLE = table;
}


void
@NAME@::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  assert (noutput_items % d_O == 0);
  int input_required =  d_D * noutput_items / d_O;
  unsigned ninputs = ninput_items_required.size();
  for (unsigned int i = 0; i < ninputs; i++)
    ninput_items_required[i] = input_required;
}



int
@NAME@::general_work (int noutput_items,
				gr_vector_int &ninput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
{

  assert (noutput_items % d_O == 0);
  assert (input_items.size() == output_items.size());
  int nstreams = input_items.size();

for (int m=0;m<nstreams;m++) {
  const @I_TYPE@ *in = (@I_TYPE@ *) input_items[m];
  float *out = (float *) output_items[m];

  for (int i = 0; i < noutput_items / d_O ; i++){
    calc_metric(d_O, d_D, d_TABLE,&(in[i*d_D]),&(out[i*d_O]), d_TYPE);
  } 
}

  consume_each (d_D * noutput_items / d_O);
  return noutput_items;
}
