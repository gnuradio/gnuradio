/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2010 Free Software Foundation, Inc.
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
#include <config.h>
#endif
#include <@NAME@.h>
#include <algorithm>
#include <gr_io_signature.h>


@NAME@::@NAME@ (int vlen)
  : gr_sync_block ("@BASE_NAME@",
		   gr_make_io_signature (1, 1, sizeof (@TYPE@) * vlen),
		   gr_make_io_signature (0, 0, 0)),
    d_vlen(vlen)
{
}

int
@NAME@::work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items)
{
  @TYPE@ *iptr = (@TYPE@ *) input_items[0];
  for (int i = 0; i < noutput_items * d_vlen; i++)
    d_data.push_back (iptr[i]);

  return noutput_items;
}


@NAME@_sptr
gr_make_@BASE_NAME@ (int vlen)
{
  return gnuradio::get_initial_sptr(new @NAME@ (vlen));
}

std::vector<@TYPE@>
@NAME@::data () const
{
  return d_data;
}
