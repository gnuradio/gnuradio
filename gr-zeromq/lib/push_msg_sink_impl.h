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

#ifndef INCLUDED_ZEROMQ_PUSH_MSG_SINK_IMPL_H
#define INCLUDED_ZEROMQ_PUSH_MSG_SINK_IMPL_H

#include <gnuradio/zeromq/push_msg_sink.h>
#include <zmq.hpp>

namespace gr {
  namespace zeromq {

    class push_msg_sink_impl : public push_msg_sink
    {
    private:
      float           d_timeout;
      zmq::context_t  *d_context;
      zmq::socket_t   *d_socket;

    public:
      push_msg_sink_impl(char *address, int timeout);
      ~push_msg_sink_impl();

      void handler(pmt::pmt_t msg);
    };

  } // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_ZMQ_PUSH_MSG_SINK_IMPL_H */
