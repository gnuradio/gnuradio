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

#ifndef INCLUDED_BLOCKS_TCP_SERVER_SINK_H
#define INCLUDED_BLOCKS_TCP_SERVER_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Send stream trought an TCP socket.
     * \ingroup networking_tools_blk
     *
     * \details
     * Listen for incoming TCP connection(s). Duplicate data for each
     * opened connection. Optionally can wait until first client connects
     * before streaming starts.
     */
    class BLOCKS_API tcp_server_sink : virtual public gr::sync_block
    {
    public:
      // gr::blocks::tcp_server_sink::sptr
      typedef boost::shared_ptr<tcp_server_sink> sptr;

      /*!
       * \brief TCP Server Sink Constructor
       *
       * \param itemsize     The size (in bytes) of the item datatype
       * \param host         The name or IP address of interface to bind to.
       * \param port         Port where to listen.
       * \param noblock      If false, wait until first client connects before
       *                     streaming starts. In non blocking mode
       *                     (noblock=true), drop data onto floor if no client
       *                     is connected.
       */
      static sptr make(size_t itemsize,
                       const std::string &host, int port,
                       bool noblock = false);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_TCP_SERVER_SINK_H */
