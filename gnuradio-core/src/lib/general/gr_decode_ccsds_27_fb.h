/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_DECODE_CCSDS_27_FB_H
#define INCLUDED_GR_DECODE_CCSDS_27_FB_H

#include <gr_core_api.h>
#include <gr_sync_decimator.h>

extern "C" {
#include <viterbi.h>
}

class gr_decode_ccsds_27_fb;

typedef boost::shared_ptr<gr_decode_ccsds_27_fb> gr_decode_ccsds_27_fb_sptr;

GR_CORE_API gr_decode_ccsds_27_fb_sptr gr_make_decode_ccsds_27_fb();

/*! \brief A rate 1/2, k=7 convolutional decoder for the CCSDS standard
 * \ingroup ecc
 *  
 * This block performs soft-decision convolutional decoding using the Viterbi
 * algorithm.
 *  
 * The input is a stream of (possibly noise corrupted) floating point values 
 * nominally spanning [-1.0, 1.0], representing the encoded channel symbols 
 * 0 (-1.0) and 1 (1.0), with erased symbols at 0.0.
 *
 * The output is MSB first packed bytes of decoded values.
 *
 * As a rate 1/2 code, there will be one output byte for every 16 input symbols.
 *
 * This block is designed for continuous data streaming, not packetized data.
 * The first 32 bits out will be zeroes, with the output delayed four bytes
 * from the corresponding inputs.
 */

class GR_CORE_API gr_decode_ccsds_27_fb : public gr_sync_decimator
{
private:
  friend GR_CORE_API gr_decode_ccsds_27_fb_sptr gr_make_decode_ccsds_27_fb();

  gr_decode_ccsds_27_fb();

  // Viterbi state
  int d_mettab[2][256];
  struct viterbi_state d_state0[64];
  struct viterbi_state d_state1[64];
  unsigned char d_viterbi_in[16];

  int d_count;
      
public:
  ~gr_decode_ccsds_27_fb();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_DECODE_CCSDS_27_FB_H */
