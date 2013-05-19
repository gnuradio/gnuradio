/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ATSC_RANDOMIZER_H
#define INCLUDED_ATSC_RANDOMIZER_H

#include <gnuradio/atsc/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/atsc/randomizer_impl.h>

class atsc_randomizer;
typedef boost::shared_ptr<atsc_randomizer> atsc_randomizer_sptr;

ATSC_API atsc_randomizer_sptr atsc_make_randomizer();

/*!
 * \brief "Whiten" incoming mpeg transport stream packets
 * \ingroup atsc
 *
 * input: atsc_mpeg_packet; output: atsc_mpeg_packet_no_sync
 */
class ATSC_API atsc_randomizer : public gr::sync_block
{
  friend ATSC_API atsc_randomizer_sptr atsc_make_randomizer();

  atsci_randomizer	d_rand;
  bool			d_field2;	// user to init plinfo in output
  int			d_segno;	// likewise

  atsc_randomizer();

public:
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  void reset();
};


#endif /* INCLUDED_ATSC_RANDOMIZER_H */
