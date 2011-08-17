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

#ifndef _ATSC_VITERBI_DECODER_H_
#define _ATSC_VITERBI_DECODER_H_

#define	USE_SIMPLE_SLICER  0

#include <atsc_api.h>
#include <atsc_types.h>
#include <interleaver_fifo.h>

#if (USE_SIMPLE_SLICER)
#include <atsci_fake_single_viterbi.h>
typedef atsci_fake_single_viterbi	single_viterbi_t;
#else
#include <atsci_single_viterbi.h>
typedef atsci_single_viterbi		single_viterbi_t;
#endif

/*!
 * \brief fancy, schmancy 12-way interleaved viterbi decoder for ATSC
 */

class ATSC_API atsci_viterbi_decoder {
public:
  static const int	NCODERS = 12;

  atsci_viterbi_decoder ();
  ~atsci_viterbi_decoder ();

  //! reset all decoder states
  void reset ();

  /*!
   * Take 12 data segments of soft decisions (floats) and
   * produce 12 RS encoded data segments.  We work in groups of 12
   * because that's the smallest number of segments that composes a
   * single full cycle of the decoder mux.
   */
  void decode (atsc_mpeg_packet_rs_encoded out[NCODERS],
	       const atsc_soft_data_segment in[NCODERS]);


  
 protected:
  typedef interleaver_fifo<unsigned char>	fifo_t;

  static const int SEGMENT_SIZE = ATSC_MPEG_RS_ENCODED_LENGTH;	// 207
  static const int OUTPUT_SIZE = (SEGMENT_SIZE * 12);
  static const int INPUT_SIZE = (ATSC_DATA_SEGMENT_LENGTH * 12);
  
  void decode_helper (unsigned char out[OUTPUT_SIZE],
		      const float in[INPUT_SIZE]);

  
  single_viterbi_t	viterbi[NCODERS];
  fifo_t		*fifo[NCODERS];	
  bool			debug;

};



#endif /* _ATSC_VITERBI_DECODER_H_ */
