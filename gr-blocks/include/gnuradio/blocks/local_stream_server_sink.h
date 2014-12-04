/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_LOCAL_STREAM_SERVER_SINK_H
#define INCLUDED_BLOCKS_LOCAL_STREAM_SERVER_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Send data trought local stream socket.
     * \ingroup networking_tools_blk
     *
     * \details
     * Listen on local socket and send data to clients. Data are
     * duplicated for each client.
     */
    class BLOCKS_API local_stream_server_sink : virtual public gr::sync_block
    {
    public:
      // gr::blocks::local_stream_server_sink::sptr
      typedef boost::shared_ptr<local_stream_server_sink> sptr;

      /*!
       * What to do when output buffers are full.
       */
      typedef enum {
        CONGESTION_BLOCK = 0,   // block until buffers are emptied
        CONGESTION_CLOSE = 1    // close connections causing congestion
      } congestion_t;

      /*!
       * \brief Local Stream Socket Server Sink Constructor
       *
       * \param itemsize     The size (in bytes) of the item datatype
       * \param address      Name of socket to bind.
       * \param noblock      If false, wait until first client connects before
       *                     streaming starts. In non blocking mode
       *                     (noblock=true), drop data onto floor if no client
       *                     is connected.
       * \param congestion   What to do when output buffers are full.
       * \param buf_size     Size of output buffer.
       */
      static sptr make(size_t itemsize, const std::string &address,
                       bool noblock = false,
                       congestion_t congestion = CONGESTION_BLOCK,
                       size_t buf_size = 128 * 1024);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_LOCAL_STREAM_SERVER_SINK_H */
