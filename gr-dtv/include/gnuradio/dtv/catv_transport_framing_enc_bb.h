/* -*- c++ -*- */
/* 
 * Copyright 2016 Free Software Foundation, Inc.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DTV_CATV_TRANSPORT_FRAMING_ENC_BB_H
#define INCLUDED_DTV_CATV_TRANSPORT_FRAMING_ENC_BB_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Transport Framing Encoder. Adds a parity checksum to MPEG-2 packets.
     * \ingroup dtv
     *
     * Input: MPEG-2 Transport Stream.\n
     * Output: MPEG-2 Transport Stream with parity checksum byte.
     */
    class DTV_API catv_transport_framing_enc_bb : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<catv_transport_framing_enc_bb> sptr;

      /*!
       * \brief Create an ITU-T J.83B Transport Framing Encoder.
       *
       */
      static sptr make();
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_TRANSPORT_FRAMING_ENC_BB_H */

