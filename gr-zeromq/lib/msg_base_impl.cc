/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
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

#include "msg_base_impl.h"

namespace gr {
  namespace zeromq {

    /*
     * Common Base Section
     */
    msg_base_impl::msg_base_impl(int type, char *address, int timeout)
      : d_zmq_started(false), type(type), d_thread(NULL)
    {
      int major, minor, patch;
      zmq::version (&major, &minor, &patch);
      if (major < 3) {
        d_timeout = timeout*1000;
      }

      d_context = new zmq::context_t(1);
      d_socket = new zmq::socket_t(*d_context, type);
    }

    msg_base_impl::~msg_base_impl()
    {
      stop();
      d_socket->close();
      delete d_socket;
      delete d_context;
    }

    void msg_base_impl::handler(pmt::pmt_t msg)
    {
      gr::thread::scoped_lock guard(d_mutex);
      std::stringbuf sb("");
      pmt::serialize(msg, sb);
      std::string s = sb.str();
      zmq::message_t zmsg(s.size());

      memcpy(zmsg.data(), s.c_str(), s.size());
      d_socket->send(zmsg);
    }

    bool msg_base_impl::start()
    {
      /*
       * This is overloaded from the gnuradio start/stop and is implemented here
       * because the "default" start/stop is either nothing or start the thread
       * for sub, pull
       */
      d_zmq_started = true;
      if (type == ZMQ_SUB || type == ZMQ_PULL) {
        d_zmq_finished = false;
        d_thread = new boost::thread(boost::bind(&msg_base_impl::readloop, this));
      }
      return true;
    }

    bool msg_base_impl::stop()
    {
      if (d_zmq_started) {
        if (type == ZMQ_SUB || type == ZMQ_PULL) {
          d_zmq_finished = true;
          d_thread->join();
          delete d_thread;
          d_thread = NULL;
        }
      }
      return true;
    }

    void msg_base_impl::readloop()
    {
      gr::thread::scoped_lock guard(d_mutex);
      while(!d_zmq_finished){
        zmq::pollitem_t items[] = { { static_cast<void *>(*d_socket), 0, ZMQ_POLLIN, 0 } };
        zmq::poll(&items[0], 1, d_timeout);

        //  If we got a reply, process
        if (items[0].revents & ZMQ_POLLIN) {

          // Receive data
          zmq::message_t msg;
          d_socket->recv(&msg);

          std::string buf(static_cast<char*>(msg.data()), msg.size());
          std::stringbuf sb(buf);
          pmt::pmt_t m = pmt::deserialize(sb);
          message_port_pub(pmt::mp("out"), m);
        } else {
          boost::this_thread::sleep(boost::posix_time::microseconds(100));
        }
      }
    }

    std::string msg_base_impl::endpoint()
    {
      size_t len = 1024;
      __GR_VLA(char, endpoint, len);
      d_socket->getsockopt(ZMQ_LAST_ENDPOINT, endpoint, &len);
      return std::string(endpoint, len-1);
    }

    /*
     * Sink Base
     */
    msg_base_sink_impl::msg_base_sink_impl(int type, char *address, int timeout)
        : msg_base_impl(type, address, timeout)
    {
      int time = 0;
      d_socket->setsockopt(ZMQ_LINGER, &time, sizeof(time));
      d_socket->bind(address);

      message_port_register_in(pmt::mp("in"));
      // ZMQ_REP has to manually check queue and wait for requests
      if (type != ZMQ_REP) {
        set_msg_handler( pmt::mp("in"),
                         boost::bind(&msg_base_sink_impl::handler, this, _1));

      }
    }

    void msg_base_sink_impl::set_endpoint(const char *address)
    {
      stop();
      gr::thread::scoped_lock guard(d_mutex);
      d_socket->close();
      delete d_socket;

      d_socket = new zmq::socket_t(*d_context, type);
      int time = 0;
      d_socket->setsockopt(ZMQ_LINGER, &time, sizeof(time));
      d_socket->bind(address);
      if (d_zmq_started) {
        start();
      }
    }

    /*
     * Source Base
     */
    msg_base_source_impl::msg_base_source_impl(int type, char *address, int timeout)
        : msg_base_impl(type, address, timeout)
    {
      d_socket->connect (address);
      message_port_register_out(pmt::mp("out"));
    }

    void msg_base_source_impl::set_endpoint(const char *address)
    {
      stop();
      gr::thread::scoped_lock guard(d_mutex);
      d_socket->close();
      delete d_socket;

      d_socket = new zmq::socket_t(*d_context, type);
      d_socket->connect(address);
      setup_socket();

      if (d_zmq_started) {
        start();
      }
    }

  }
}