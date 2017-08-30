/* -*- c++ -*- */
/*
 * Copyright 2013-2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ZEROMQ_REQ_MSG_SOURCE_H
#define INCLUDED_ZEROMQ_REQ_MSG_SOURCE_H

#include <gnuradio/zeromq/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace zeromq {

    /*!
     * \brief Receive messages on ZMQ REQ socket output async messages
     * \ingroup zeromq
     *
     * \details
     * This block will connect to a ZMQ REP socket, then resend all
     * incoming messages as asynchronous messages.
     */
    class ZEROMQ_API req_msg_source : virtual public gr::block
    {
    public:
      typedef boost::shared_ptr<req_msg_source> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of zeromq::req_msg_source.
       *
       * \param address  ZMQ socket address specifier
       * \param timeout  Receive timeout in milliseconds, default is 100ms, 1us increments
       *
       */
      static sptr make(char *address, int timeout=100);
    };

  } // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_SOURCE_REQREP_H */
