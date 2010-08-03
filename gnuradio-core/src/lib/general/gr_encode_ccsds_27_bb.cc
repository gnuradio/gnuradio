/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2010 Free Software Foundation, Inc.
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

#include <gr_encode_ccsds_27_bb.h>
#include <gr_io_signature.h>

extern "C" {
#include <../viterbi/viterbi.h>
}

gr_encode_ccsds_27_bb_sptr 
gr_make_encode_ccsds_27_bb()
{
  return gnuradio::get_initial_sptr(new gr_encode_ccsds_27_bb());
}

gr_encode_ccsds_27_bb::gr_encode_ccsds_27_bb()
  : gr_sync_interpolator("encode_ccsds_27_bb",
		         gr_make_io_signature(1, 1, sizeof(char)),
		         gr_make_io_signature(1, 1, sizeof(char)),
			 16)  // Rate 1/2 code, packed to unpacked conversion
{
  d_encstate = 0;
}

gr_encode_ccsds_27_bb::~gr_encode_ccsds_27_bb()
{
}

int 
gr_encode_ccsds_27_bb::work(int noutput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items)
{
  unsigned char *in = (unsigned char *)input_items[0];
  unsigned char *out = (unsigned char *)output_items[0];

  d_encstate = encode(out, in, noutput_items/16, d_encstate);

  return noutput_items;
}
