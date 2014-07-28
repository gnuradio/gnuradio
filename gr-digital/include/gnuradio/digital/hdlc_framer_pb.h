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

#ifndef INCLUDED_DIGITAL_HDLC_FRAMER_PB_H
#define INCLUDED_DIGITAL_HDLC_FRAMER_PB_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief HDLC framer which takes in PMT binary blobs and outputs HDLC
     * frames as unpacked bits, with CRC and bit stuffing added. The first sample
     * of the frame is tagged with the tag frame_tag_name and includes a
     * length field for tagged_stream use.
     *
     * This block outputs one whole frame at a time; if there is not enough
     * output buffer space to fit a frame, it is pushed onto a queue. As a result
     * flowgraphs which only run for a finite number of samples may not receive
     * all frames in the queue, due to the scheduler's granularity. For
     * flowgraphs that stream continuously (anything using a USRP) this should
     * not be an issue.
     *
     * \ingroup pkt_operators_blk
     *
     */
    class DIGITAL_API hdlc_framer_pb : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<hdlc_framer_pb> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of digital::hdlc_framer.
       *
       * \param frame_tag_name: The tag to add to the first sample of each frame.
       */
      static sptr make(const std::string frame_tag_name);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HDLC_FRAMER_PB_H */

