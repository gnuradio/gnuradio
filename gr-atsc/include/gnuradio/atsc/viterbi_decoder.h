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

#ifndef INCLUDED_ATSC_VITERBI_DECODER_H
#define INCLUDED_ATSC_VITERBI_DECODER_H

#include <gnuradio/atsc/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/atsc/api.h>
#include <gnuradio/atsc/types.h>
#include <gnuradio/atsc/interleaver_fifo.h>

#define	USE_SIMPLE_SLICER  0

#if (USE_SIMPLE_SLICER)
#include <gnuradio/atsc/fake_single_viterbi_impl.h>
typedef atsci_fake_single_viterbi	single_viterbi_t;
#else
#include <gnuradio/atsc/single_viterbi_impl.h>
typedef atsci_single_viterbi		single_viterbi_t;
#endif

class atsc_viterbi_decoder;
typedef boost::shared_ptr<atsc_viterbi_decoder> atsc_viterbi_decoder_sptr;

ATSC_API atsc_viterbi_decoder_sptr atsc_make_viterbi_decoder();

/*!
 * \brief ATSC 12-way interleaved viterbi decoder (atsc_soft_data_segment --> atsc_mpeg_packet_rs_encoded)
 * \ingroup atsc
 *
 * input: atsc_soft_data_segment; output: atsc_mpeg_packet_rs_encoded
 */
class ATSC_API atsc_viterbi_decoder : public gr::sync_block
{
  friend ATSC_API atsc_viterbi_decoder_sptr atsc_make_viterbi_decoder();

public:
  atsc_viterbi_decoder ();
  ~atsc_viterbi_decoder ();

  static const int	NCODERS = 12;
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  void reset();

protected:
  int	    last_start;
  typedef interleaver_fifo<unsigned char>	fifo_t;

  static const int SEGMENT_SIZE = ATSC_MPEG_RS_ENCODED_LENGTH;	// 207
  static const int OUTPUT_SIZE = (SEGMENT_SIZE * 12);
  static const int INPUT_SIZE = (ATSC_DATA_SEGMENT_LENGTH * 12);

  single_viterbi_t	viterbi[NCODERS];
  fifo_t		*fifo[NCODERS];
  bool			debug;

};


#endif /* INCLUDED_ATSC_VITERBI_DECODER_H */
