/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_PACKET_SINK_H
#define INCLUDED_GR_PACKET_SINK_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/msg_queue.h>

namespace gr {
  namespace digital {

    /*!
     * \brief process received bits looking for packet sync, header,
     * and process bits into packet
     * \ingroup packet_operators_blk
     *
     * \details
     * input: stream of symbols to be sliced.
     * 
     * output: none. Pushes assembled packet into target queue
     *
     * The packet sink takes in a stream of binary symbols that are
     * sliced around 0. The bits are then checked for the \p
     * sync_vector to determine find and decode the packet. It then
     * expects a fixed length header of 2 16-bit shorts containing the
     * payload length, followed by the payload. If the 2 16-bit shorts
     * are not identical, this packet is ignored. Better algs are
     * welcome.
     *
     * This block is not very useful anymore as it only works with
     * 2-level modulations such as BPSK or GMSK. The block can
     * generally be replaced with a correlate access code and frame
     * sink blocks.
     */
    class DIGITAL_API packet_sink : virtual public sync_block
    {
    public:
      // gr::digital::packet_sink::sptr
      typedef boost::shared_ptr<packet_sink> sptr;

      /*!
       * Make a packet_sink block.
       *
       * \param sync_vector The synchronization vector as a vector of 1's and 0's.
       * \param target_queue The message queue that packets are sent to.
       * \param threshold Number of bits that can be incorrect in the \p sync_vector.
       */
      static sptr make(const std::vector<unsigned char>& sync_vector,
		       msg_queue::sptr target_queue,
		       int threshold=-1);

      //! return true if we detect carrier
      virtual bool carrier_sensed() const = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_PACKET_SINK_H */
