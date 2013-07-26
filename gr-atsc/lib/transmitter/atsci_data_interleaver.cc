/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#include <gnuradio/atsc/data_interleaver_impl.h>

void
atsci_data_interleaver::interleave (atsc_mpeg_packet_rs_encoded &out,
				   const atsc_mpeg_packet_rs_encoded &in)
{
  assert (in.pli.regular_seg_p ());
  plinfo::sanity_check (in.pli);

  out.pli = in.pli;			// copy pipeline info
  if (in.pli.first_regular_seg_p ())	// reset commutator if required
    sync ();

  transform (out.data, in.data, sizeof (in.data));
}


void
atsci_data_deinterleaver::deinterleave (atsc_mpeg_packet_rs_encoded &out,
				       const atsc_mpeg_packet_rs_encoded &in)
{
  assert (in.pli.regular_seg_p ());
  plinfo::sanity_check (in.pli);

  // reset commutator if required using INPUT pipeline info
  if (in.pli.first_regular_seg_p ())
    sync ();

  // remap OUTPUT pipeline info to reflect 52 data segment end-to-end delay

  plinfo::delay (out.pli, in.pli, 52);

  // now do the actual deinterleaving

  for (unsigned int i = 0; i < sizeof (in.data); i++){
    out.data[i] = alignment_fifo.stuff (transform (in.data[i]));
  }
}

