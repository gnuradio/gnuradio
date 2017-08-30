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

#ifndef INCLUDED_ZEROMQ_PULL_SOURCE_H
#define INCLUDED_ZEROMQ_PULL_SOURCE_H

#include <gnuradio/zeromq/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace zeromq {

    /*!
     * \brief Receive messages on ZMQ PULL socket and source stream
     * \ingroup zeromq
     *
     * \details
     * This block will connect to a ZMQ PUSH socket, then produce all
     * incoming messages as streaming output.
     */
    class ZEROMQ_API pull_source : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<pull_source> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of gr::zeromq::pull_source.
       *
       * \param itemsize Size of a stream item in bytes.
       * \param vlen Vector length of the input items. Note that one vector is one item.
       * \param address  ZMQ socket address specifier.
       * \param timeout  Receive timeout in milliseconds, default is 100ms, 1us increments.
       * \param pass_tags Whether source will look for and deserialize tags.
       * \param hwm High Watermark to configure the socket to (-1 => zmq's default)
       */
      static sptr make(size_t itemsize, size_t vlen, char *address,
                       int timeout=100, bool pass_tags=false, int hwm=-1);
    };

  } // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_PULL_SOURCE_H */
