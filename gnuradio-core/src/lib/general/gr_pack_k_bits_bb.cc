/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_pack_k_bits_bb.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <iostream>

gr_pack_k_bits_bb_sptr gr_make_pack_k_bits_bb(unsigned k)
{
  return gnuradio::get_initial_sptr(new gr_pack_k_bits_bb(k));
}


gr_pack_k_bits_bb::gr_pack_k_bits_bb (unsigned k)
  : gr_sync_decimator("pack_k_bits_bb",
		      gr_make_io_signature (1, 1, sizeof(unsigned char)),
		      gr_make_io_signature (1, 1, sizeof(unsigned char)),
		      k),
    d_k (k)
{
  if (d_k == 0)
    throw std::out_of_range("interpolation must be > 0");
}

gr_pack_k_bits_bb::~gr_pack_k_bits_bb()
{
}

int
gr_pack_k_bits_bb::work(int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  const unsigned char *in = (const unsigned char *)input_items[0];
  unsigned char *out = (unsigned char *)output_items[0];

  for(int i = 0; i < noutput_items; i++) {
    out[i] = 0x00;
    for(unsigned int j = 0; j < d_k; j++) {
        out[i] |= (0x01 & in[i*d_k+j])<<(d_k-j-1);
    }
  }

  return noutput_items;
}
