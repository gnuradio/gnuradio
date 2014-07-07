/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FEC_DECODE_CCSDS_27_FB_H
#define INCLUDED_FEC_DECODE_CCSDS_27_FB_H

#include <gnuradio/fec/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace fec {

    /*! \brief A rate 1/2, k=7 convolutional decoder for the CCSDS standard
     * \ingroup error_coding_blk
     *
     * \details
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

    class FEC_API decode_ccsds_27_fb : virtual public sync_decimator
    {
    public:

      // gr::fec::decode_ccsds_27_fb::sptr
      typedef boost::shared_ptr<decode_ccsds_27_fb> sptr;

      static sptr make();
    };

  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_DECODE_CCSDS_27_FB_H */
