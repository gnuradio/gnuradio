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

#ifndef INCLUDED_FEC_ENCODE_CCSDS_27_BB_H
#define INCLUDED_FEC_ENCODE_CCSDS_27_BB_H

#include <gnuradio/fec/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
  namespace fec {

    /*! \brief A rate 1/2, k=7 convolutional encoder for the CCSDS standard
     * \ingroup error_coding_blk
     *
     * \details
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

    class FEC_API encode_ccsds_27_bb : virtual public sync_interpolator
    {
    public:

      // gr::fec::encode_ccsds_27_bb::sptr
      typedef boost::shared_ptr<encode_ccsds_27_bb> sptr;

      static sptr make();
    };

  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_ENCODE_CCSDS_27_BB_H */
