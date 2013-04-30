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

#include <gnuradio/atsc/viterbi_decoder.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/atsc/consts.h>
#include <iostream>

using std::cerr;
using std::endl;

atsc_viterbi_decoder_sptr
atsc_make_viterbi_decoder()
{
  return gnuradio::get_initial_sptr(new atsc_viterbi_decoder());
}

atsc_viterbi_decoder::atsc_viterbi_decoder()
  : gr::sync_block("atsc_viterbi_decoder",
		  gr::io_signature::make(1, 1, sizeof(atsc_soft_data_segment)),
		  gr::io_signature::make(1, 1, sizeof(atsc_mpeg_packet_rs_encoded))),
		  last_start(-1)
{
  set_output_multiple(atsci_viterbi_decoder::NCODERS);
  reset();
}

int
atsc_viterbi_decoder::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  const atsc_soft_data_segment *in = (const atsc_soft_data_segment *) input_items[0];
  atsc_mpeg_packet_rs_encoded *out = (atsc_mpeg_packet_rs_encoded *) output_items[0];

  assert (noutput_items % atsci_viterbi_decoder::NCODERS == 0);

  // find the first mod 12 boundary to begin decoding
  int start;
  for (start = 0; start < atsci_viterbi_decoder::NCODERS; start++){
    assert (in[start].pli.regular_seg_p ());
    if ((in[start].pli.segno () % atsci_viterbi_decoder::NCODERS) == 0)
      break;
  }

  if (start == atsci_viterbi_decoder::NCODERS){
    // we didn't find a mod 12 boundary.  There's some kind of problem
    // upstream of us (not yet sync'd??)
    cerr << "!!!atsc_viterbi_decoder: no mod-12 boundary found\7\n";
    start = 0;
  }
  else if (start != last_start){
    cerr << "atsc_viterbi_decoder: new starting offset = " << start
         << endl;
    last_start = start;
  }

  for (int i = 0; i < atsci_viterbi_decoder::NCODERS; i += atsci_viterbi_decoder::NCODERS){
    d_viterbi_decoder.decode(&out[i], &in[i + start]);
  }
  return atsci_viterbi_decoder::NCODERS;
}

