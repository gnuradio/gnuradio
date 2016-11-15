/* -*- c++ -*- */
/*
 * Copyright 2007,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_OFDM_SAMPLER_H
#define INCLUDED_DIGITAL_OFDM_SAMPLER_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace digital {
    
    /*!
     * \brief does the rest of the OFDM stuff
     * \ingroup ofdm_blk
     * \ingroup deprecated_blk
     */
    class DIGITAL_API ofdm_sampler : virtual public block
    {
    public:
      // gr::digital::ofdm_sampler::sptr
      typedef boost::shared_ptr<ofdm_sampler> sptr;

      /*!
       * Make an OFDM sampler block.
       *
       * \param fft_length The size of the FFT vector (occupied_carriers + unused carriers)
       * \param symbol_length Length of the full symbol (fft_length + CP length)
       * \param timeout timeout in samples when we stop looking for a symbol after initial ack.
       */
      static sptr make(unsigned int fft_length, 
		       unsigned int symbol_length,
		       unsigned int timeout=1000);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_OFDM_SAMPLER_H */
