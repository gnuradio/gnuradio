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

// @WARNING@

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <@NAME@.h>
#include <gr_io_signature.h>

@SPTR_NAME@ 
gr_make_@BASE_NAME@ (int length, @O_TYPE@ scale, int max_iter)
{
  return gnuradio::get_initial_sptr (new @NAME@ (length, scale, max_iter));
}

@NAME@::@NAME@ (int length, @O_TYPE@ scale, int max_iter)
  : gr_sync_block ("@BASE_NAME@",
		   gr_make_io_signature (1, 1, sizeof (@I_TYPE@)),
		   gr_make_io_signature (1, 1, sizeof (@O_TYPE@))),
  d_length(length),
  d_scale(scale),
  d_max_iter(max_iter),
  d_new_length(length),
  d_new_scale(scale),
  d_updated(false)
{
  set_history(length);
}

@NAME@::~@NAME@ ()
{
}

void
@NAME@::set_length_and_scale(int length, @O_TYPE@ scale)
{
  d_new_length = length;
  d_new_scale = scale;
  d_updated = true;
}

int 
@NAME@::work (int noutput_items,
	      gr_vector_const_void_star &input_items,
	      gr_vector_void_star &output_items)
{
  if (d_updated) {
    d_length = d_new_length;
    d_scale = d_new_scale;
    set_history(d_length);
    d_updated = false;
    return 0; // history requirements might have changed
  }

  const @I_TYPE@ *in = (const @I_TYPE@ *) input_items[0];
  @O_TYPE@ *out = (@O_TYPE@ *) output_items[0];

  @I_TYPE@ sum = 0;
  int num_iter = (noutput_items>d_max_iter) ? d_max_iter : noutput_items;
  for (int i = 0; i < d_length-1 ; i++) {
    sum += in[i];
  }

  for (int i = 0; i < num_iter; i++) {
    sum += in[i+d_length-1];
    out[i] = sum * d_scale;
    sum -= in[i];
  }

  return num_iter;
}
