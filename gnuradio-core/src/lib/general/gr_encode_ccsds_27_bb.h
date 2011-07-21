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
#ifndef INCLUDED_GR_ENCODE_CCSDS_27_BB_H
#define INCLUDED_GR_ENCODE_CCSDS_27_BB_H

#include <gr_core_api.h>
#include <gr_sync_interpolator.h>

class gr_encode_ccsds_27_bb;

typedef boost::shared_ptr<gr_encode_ccsds_27_bb> gr_encode_ccsds_27_bb_sptr;

GR_CORE_API gr_encode_ccsds_27_bb_sptr gr_make_encode_ccsds_27_bb();

/*! \brief A rate 1/2, k=7 convolutional encoder for the CCSDS standard
 * \ingroup ecc
 *  
 * This block performs convolutional encoding using the CCSDS standard
 * polynomial ("Voyager").
 *  
 * The input is an MSB first packed stream of bits.
 *
 * The output is a stream of symbols 0 or 1 representing the encoded data.
 *
 * As a rate 1/2 code, there will be 16 output symbols for every input byte.
 *
 * This block is designed for continuous data streaming, not packetized data.
 * There is no provision to "flush" the encoder.
 */

class GR_CORE_API gr_encode_ccsds_27_bb : public gr_sync_interpolator
{
private:
  friend GR_CORE_API gr_encode_ccsds_27_bb_sptr gr_make_encode_ccsds_27_bb();

  gr_encode_ccsds_27_bb();
  unsigned char d_encstate;
  
 public:
  ~gr_encode_ccsds_27_bb();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_ENCODE_CCSDS_27_BB_H */
