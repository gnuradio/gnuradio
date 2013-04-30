/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CONSTELLATION_DECODER_CB_H
#define INCLUDED_DIGITAL_CONSTELLATION_DECODER_CB_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Constellation Decoder
     * \ingroup symbol_coding_blk
     *
     * \details
     * Decode a constellation's points from a complex space to
     * (unpacked) bits based on the map of the \p consetllation
     * object.
     */
    class DIGITAL_API constellation_decoder_cb
      : virtual public block
    {
    public:
      // gr::digital::constellation_decoder_cb::sptr
      typedef boost::shared_ptr<constellation_decoder_cb> sptr;

      /*!
       * \brief Make constellation decoder block.
       *
       * \param constellation A constellation derived from class
       * 'constellation'. Use base() method to get a shared pointer to
       * this base class type.
       */
      static sptr make(constellation_sptr constellation);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CONSTELLATION_DECODER_CB_H */
