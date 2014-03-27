/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_HDLC_DEFRAMER_BP_H
#define INCLUDED_DIGITAL_HDLC_DEFRAMER_BP_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief HDLC deframer which takes in unpacked bits, and outputs PDU 
     * binary blobs. This is intended for use with the 
     * correlate_access_code_tag_bb block, which should have an access code
     * of "01111110" for use with HDLC frames. Frames which do not pass CRC are
     * rejected.
     *
     * \ingroup digital
     *
     */
    class DIGITAL_API hdlc_deframer_bp : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<hdlc_deframer_bp> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of digital::hdlc_deframer.
       *
       * \param frame_tag_name: The tag name from an upstream 
       * correlate_access_code_tag_bb block.
       * \param length_min: Minimum frame size (default: 32)
       * \param length_max: Maximum frame size (default: 500)
       */
      static sptr make(const std::string frame_tag_name, int length_min, int length_max);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HDLC_DEFRAMER_BP_H */

