/* -*- c++ -*- */
/*
 * Copyright 2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
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

#ifndef INCLUDED_ZEROMQ_PUB_SINK_H
#define INCLUDED_ZEROMQ_PUB_SINK_H

#include <gnuradio/zeromq/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace zeromq {

    /*!
     * \brief Sink the contents of a stream to a ZMQ PUB socket
     * \ingroup zeromq
     *
     * \details
     * This block acts a a streaming sink for a GNU Radio flowgraph
     * and writes its contents to a ZMQ PUB socket.  A PUB socket may
     * have subscribers and will pass all incoming stream data to each
     * subscriber.  Subscribers can be either another gr-zeromq source
     * block or a non-GNU Radio ZMQ socket.
     */
    class ZEROMQ_API pub_sink : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<pub_sink> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of zeromq::pub_sink.
       *
       * \param itemsize Size of a stream item in bytes.
       * \param vlen Vector length of the input items. Note that one vector is one item.
       * \param address  ZMQ socket address specifier.
       * \param timeout  Receive timeout in milliseconds, default is 100ms, 1us increments.
       * \param pass_tags Whether sink will serialize and pass tags over the link.
       * \param hwm High Watermark to configure the socket to (-1 => zmq's default)
       */
      static sptr make(size_t itemsize, size_t vlen, char *address,
                       int timeout=100, bool pass_tags=false, int hwm=-1);
    };

  } // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_PUB_SINK_H */
