/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_OFDM_MAPPER_BCV_H
#define INCLUDED_DIGITAL_OFDM_MAPPER_BCV_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/msg_queue.h>

namespace gr {
  namespace digital {

    /*!
     * \brief take a stream of bytes in and map to a vector of complex
     * constellation points suitable for IFFT input to be used in an
     * ofdm modulator.
     * \ingroup ofdm_blk
     * \ingroup deprecated_blk
     *
     * \details
     * Abstract class must be subclassed with specific mapping.
     */
    class DIGITAL_API ofdm_mapper_bcv : virtual public sync_block
    {
    public:
      // gr::digital::ofdm_mapper_bcv::sptr
      typedef boost::shared_ptr<ofdm_mapper_bcv> sptr;

      /*!
       * Make an OFDM mapper block.
       *
       * \param constellation vector of OFDM carrier symbols in complex space
       * \param msgq_limit limit on number of messages the queue can store
       * \param occupied_carriers The number of subcarriers with data in the received symbol
       * \param fft_length The size of the FFT vector (occupied_carriers + unused carriers)
       */
      static sptr make(const std::vector<gr_complex> &constellation,
		       unsigned msgq_limit, 
		       unsigned occupied_carriers,
		       unsigned int fft_length);

      virtual msg_queue::sptr msgq() const = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_OFDM_MAPPER_BCV_H */
