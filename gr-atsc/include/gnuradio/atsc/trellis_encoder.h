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
#ifndef INCLUDED_ATSC_TRELLIS_ENCODER_H
#define INCLUDED_ATSC_TRELLIS_ENCODER_H

#include <gnuradio/atsc/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/atsc/trellis_encoder_impl.h>

class atsc_trellis_encoder;
typedef boost::shared_ptr<atsc_trellis_encoder> atsc_trellis_encoder_sptr;

ATSC_API atsc_trellis_encoder_sptr atsc_make_trellis_encoder();

/*!
 * \brief ATSC 12-way interleaved trellis encoder (atsc_mpeg_packet_rs_encoded --> atsc_data_segment)
 * \ingroup atsc
 *
 * input: atsc_mpeg_packet_rs_encoded; output: atsc_data_segment
 */
class ATSC_API atsc_trellis_encoder : public gr::sync_block
{
  friend ATSC_API atsc_trellis_encoder_sptr atsc_make_trellis_encoder();

  atsci_trellis_encoder	d_trellis_encoder;

  atsc_trellis_encoder();

public:
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  void reset() { /* nop */ }
};


#endif /* INCLUDED_ATSC_TRELLIS_ENCODER_H */
