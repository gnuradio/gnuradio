/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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
gr_make_@BASE_NAME@ (size_t nitems_per_block)
{
  return @SPTR_NAME@ (new @NAME@ (nitems_per_block));
}

@NAME@::@NAME@ (size_t nitems_per_block)
  : gr_sync_block ("@BASE_NAME@",
		   gr_make_io_signature (1, -1, sizeof (@I_TYPE@)*nitems_per_block),
		   gr_make_io_signature (1,  1, sizeof (@O_TYPE@)*nitems_per_block))
{
}

int
@NAME@::work (int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
{
  @O_TYPE@ *optr = (@O_TYPE@ *) output_items[0];

  int ninputs = input_items.size ();
  int nitems_per_block = output_signature()->sizeof_stream_item(0)/sizeof(@I_TYPE@);

  for (int i = 0; i < noutput_items; i++){
    for (int j = 0; j < nitems_per_block; j++){
      @I_TYPE@ acc = ((@I_TYPE@ *) input_items[0])[i*nitems_per_block+j];
      for (int k = 1; k < ninputs; k++)
        acc += ((@I_TYPE@ *) input_items[k])[i*nitems_per_block+j];

      *optr++ = (@O_TYPE@) acc;
    }
  }
  return noutput_items;
}
