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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INCLUDED_DIGITAL_OFDM_INSERT_PREAMBLE_H
#define INCLUDED_DIGITAL_OFDM_INSERT_PREAMBLE_H

#include <gnuradio/digital/api.h>
#include <gnuradio/block.h>
#include <vector>

namespace gr {
  namespace digital {
    
    /*!
     * \brief insert "pre-modulated" preamble symbols before each payload.
     * \ingroup ofdm_blk
     * \ingroup synchronizers_blk
     * \ingroup deprecated_blk
     *
     * \details
     * <pre>
     * input 1: stream of vectors of gr_complex [fft_length]
     *          These are the modulated symbols of the payload.
     *
     * input 2: stream of char.  The LSB indicates whether the corresponding
     *          symbol on input 1 is the first symbol of the payload or not.
     *          It's a 1 if the corresponding symbol is the first symbol,
     *          otherwise 0.
     *
     * N.B., this implies that there must be at least 1 symbol in the payload.
     *
     * output 1: stream of vectors of gr_complex [fft_length]
     *           These include the preamble symbols and the payload symbols.
     *
     * output 2: stream of char.  The LSB indicates whether the corresponding
     *           symbol on input 1 is the first symbol of a packet (i.e., the
     *           first symbol of the preamble.)   It's a 1 if the corresponding
     *           symbol is the first symbol, otherwise 0.
     * </pre>
     */
    class DIGITAL_API ofdm_insert_preamble : virtual public block
    {
    public:
      // gr::digital::ofdm_insert_preamble::sptr
      typedef boost::shared_ptr<ofdm_insert_preamble> sptr;
      
      /*!
       * Make an OFDM preamble inserter block.
       *
       * \param fft_length length of each symbol in samples.
       * \param preamble   vector of symbols that represent the pre-modulated preamble.
       */
      static sptr make(int fft_length,
		       const std::vector<std::vector<gr_complex> > &preamble);

      virtual void enter_preamble() = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_OFDM_INSERT_PREAMBLE_H */
