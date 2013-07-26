/* -*- c++ -*- */
/*
 * Copyright 2006,2010 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <gnuradio/atsc/deinterleaver.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/atsc/consts.h>


atsc_deinterleaver_sptr
atsc_make_deinterleaver()
{
  return gnuradio::get_initial_sptr(new atsc_deinterleaver());
}

atsc_deinterleaver::atsc_deinterleaver()
  : gr::sync_block("atsc_deinterleaver",
		  gr::io_signature::make(1, 1, sizeof(atsc_mpeg_packet_rs_encoded)),
		  gr::io_signature::make(1, 1, sizeof(atsc_mpeg_packet_rs_encoded)))
{
  reset();
}

int
atsc_deinterleaver::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  const atsc_mpeg_packet_rs_encoded *in = (const atsc_mpeg_packet_rs_encoded *) input_items[0];
  atsc_mpeg_packet_rs_encoded *out = (atsc_mpeg_packet_rs_encoded *) output_items[0];

  for (int i = 0; i < noutput_items; i++){
    d_deinterleaver.deinterleave (out[i], in[i]);
  }

  return noutput_items;
}
