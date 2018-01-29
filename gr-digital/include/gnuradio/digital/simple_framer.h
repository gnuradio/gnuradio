/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_SIMPLE_FRAMER_H
#define INCLUDED_GR_SIMPLE_FRAMER_H

#include <gnuradio/digital/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief add sync field, seq number and command field to payload
     * \ingroup packet_operators_blk
     * \ingroup deprecated_blk
     *
     * \details
     * Takes in enough samples to create a full output frame. The
     * frame is prepended with the GRSF_SYNC (defined in
     * simple_framer_sync.h) and an 8-bit sequence number.
     */
    class DIGITAL_API simple_framer : virtual public block
    {
    public:
      // gr::digital::simple_framer::sptr
      typedef boost::shared_ptr<simple_framer> sptr;

      /*!
       * Make a simple_framer block.
       *
       * \param payload_bytesize The size of the payload in bytes.
       */
      static sptr make(int payload_bytesize);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_SIMPLE_FRAMER_H */
