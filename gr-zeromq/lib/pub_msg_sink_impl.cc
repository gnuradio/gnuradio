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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "pub_msg_sink_impl.h"
#include "tag_headers.h"

namespace gr {
  namespace zeromq {

    pub_msg_sink::sptr
    pub_msg_sink::make(char *address, int timeout)
    {
      return gnuradio::get_initial_sptr
        (new pub_msg_sink_impl(address, timeout));
    }

    pub_msg_sink_impl::pub_msg_sink_impl(char *address, int timeout)
      : gr::block("pub_msg_sink",
                  gr::io_signature::make(0, 0, 0),
                  gr::io_signature::make(0, 0, 0)),
        d_timeout(timeout)
    {
      int major, minor, patch;
      zmq::version (&major, &minor, &patch);
      if (major < 3) {
        d_timeout = timeout*1000;
      }

      d_context = new zmq::context_t(1);
      d_socket = new zmq::socket_t(*d_context, ZMQ_PUB);
      int time = 0;
      d_socket->setsockopt(ZMQ_LINGER, &time, sizeof(time));
      d_socket->bind(address);

      message_port_register_in(pmt::mp("in"));
      set_msg_handler( pmt::mp("in"),
        boost::bind(&pub_msg_sink_impl::handler, this, _1));
    }

    pub_msg_sink_impl::~pub_msg_sink_impl()
    {
      d_socket->close();
      delete d_socket;
      delete d_context;
    }

    void pub_msg_sink_impl::handler(pmt::pmt_t msg)
    {
      std::stringbuf sb("");
      pmt::serialize(msg, sb);
      std::string s = sb.str();
      zmq::message_t zmsg(s.size());

      memcpy(zmsg.data(), s.c_str(), s.size());
      d_socket->send(zmsg);
    }

  } /* namespace zeromq */
} /* namespace gr */
