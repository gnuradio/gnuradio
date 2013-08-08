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

#ifndef INCLUDED_ATSC_RS_DECODER_H
#define INCLUDED_ATSC_RS_DECODER_H

#include <gnuradio/atsc/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/atsc/reed_solomon_impl.h>

class atsc_rs_decoder;
typedef boost::shared_ptr<atsc_rs_decoder> atsc_rs_decoder_sptr;

ATSC_API atsc_rs_decoder_sptr atsc_make_rs_decoder();

/*!
 * \brief Reed-Solomon decoder for ATSC
 * \ingroup atsc
 *
 * input: atsc_mpeg_packet_rs_encoded; output: atsc_mpeg_packet_no_sync
 */
class ATSC_API atsc_rs_decoder : public gr::sync_block
{
  friend ATSC_API atsc_rs_decoder_sptr atsc_make_rs_decoder();

  atsci_reed_solomon	d_rs_decoder;

  atsc_rs_decoder();

public:
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  void reset() { /* nop */ }
};


#endif /* INCLUDED_ATSC_RS_DECODER_H */
