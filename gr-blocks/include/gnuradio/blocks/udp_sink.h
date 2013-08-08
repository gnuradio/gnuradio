/* -*- c++ -*- */
/*
 * Copyright 2007-2010,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_UDP_SINK_H
#define INCLUDED_GR_UDP_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Write stream to an UDP socket.
     * \ingroup networking_tools_blk
     */
    class BLOCKS_API udp_sink : virtual public sync_block
    {
    public:
      // gr::blocks::udp_sink::sptr
      typedef boost::shared_ptr<udp_sink> sptr;

      /*!
       * \brief UDP Sink Constructor
       *
       * \param itemsize     The size (in bytes) of the item datatype
       * \param host         The name or IP address of the receiving host; use
       *                     NULL or None for no connection
       * \param port         Destination port to connect to on receiving host
       * \param payload_size UDP payload size by default set to
       *                     1472 = (1500 MTU - (8 byte UDP header) - (20 byte IP header))
       * \param eof          Send zero-length packet on disconnect
       */
      static sptr make(size_t itemsize,
                       const std::string &host, int port,
                       int payload_size=1472, bool eof=true);

      /*! \brief return the PAYLOAD_SIZE of the socket */
      virtual int payload_size() = 0;

      /*! \brief Change the connection to a new destination
       *
       * \param host         The name or IP address of the receiving host; use
       *                     NULL or None to break the connection without closing
       * \param port         Destination port to connect to on receiving host
       *
       * Calls disconnect() to terminate any current connection first.
       */
      virtual void connect(const std::string &host, int port) = 0;

      /*! \brief Send zero-length packet (if eof is requested) then stop sending
       *
       * Zero-byte packets can be interpreted as EOF by gr_udp_source.
       * Note that disconnect occurs automatically when the sink is
       * destroyed, but not when its top_block stops.*/
      virtual void disconnect() = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_UDP_SINK_H */
