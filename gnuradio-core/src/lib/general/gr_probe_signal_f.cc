/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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
#include <gr_probe_signal_f.h>
#include <gr_io_signature.h>

gr_probe_signal_f_sptr
gr_make_probe_signal_f()
{
  return gr_probe_signal_f_sptr(new gr_probe_signal_f());
}

gr_probe_signal_f::gr_probe_signal_f ()
  : gr_sync_block ("probe_signal_f",
		   gr_make_io_signature(1, 1, sizeof(float)),
		   gr_make_io_signature(0, 0, 0)),
    d_level(0)
{
}

gr_probe_signal_f::~gr_probe_signal_f()
{
}


int
gr_probe_signal_f::work(int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];

  if (noutput_items > 0)
    d_level = in[noutput_items-1];

  return noutput_items;
}

