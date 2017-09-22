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

#ifndef INCLUDED_ZEROMQ_REP_MSG_SINK_H
#define INCLUDED_ZEROMQ_REP_MSG_SINK_H

#include <gnuradio/zeromq/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace zeromq {

    /*!
     * \brief Sink the contents of a msg port to a ZMQ REP socket
     * \ingroup zeromq
     *
     * \details
     * This block acts a message port receiver and writes individual
     * messages to a ZMQ REP socket.  The corresponding receiving ZMQ
     * REQ socket can be either another gr-zeromq source block or a
     * non-GNU Radio ZMQ socket.
     */
    class ZEROMQ_API rep_msg_sink : virtual public gr::block
    {
    public:
      typedef boost::shared_ptr<rep_msg_sink> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of zeromq::rep_msg_sink.
       *
       * \param address  ZMQ socket address specifier
       * \param timeout  Receive timeout in milliseconds, default is 100ms, 1us increments
       *
       */
      static sptr make(char *address, int timeout=100);
    };

  } // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_REP_MSG_SINK_H */
