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
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "sub_msg_source_impl.h"
#include "tag_headers.h"

namespace gr {
  namespace zeromq {

    sub_msg_source::sptr
    sub_msg_source::make(char *address, int timeout)
    {
      return gnuradio::get_initial_sptr
        (new sub_msg_source_impl(address, timeout));
    }

    sub_msg_source_impl::sub_msg_source_impl(char *address, int timeout)
      : gr::block("sub_msg_source",
                  gr::io_signature::make(0, 0, 0),
                  gr::io_signature::make(0, 0, 0)),
      d_timeout(timeout),
      d_port(pmt::mp("out"))
    {
      int major, minor, patch;
      zmq::version(&major, &minor, &patch);

      if (major < 3) {
        d_timeout = timeout*1000;
      }

      d_context = new zmq::context_t(1);
      d_socket = new zmq::socket_t(*d_context, ZMQ_SUB);

      d_socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
      d_socket->connect (address);

      message_port_register_out(d_port);
    }

    sub_msg_source_impl::~sub_msg_source_impl()
    {
      d_socket->close();
      delete d_socket;
      delete d_context;
    }

    bool sub_msg_source_impl::start()
    {
      d_finished = false;
      d_thread = new boost::thread(boost::bind(&sub_msg_source_impl::readloop, this));
      return true;
    }

    bool sub_msg_source_impl::stop()
    {
      d_finished = true;
      d_thread->join();
      return true;
    }

    void sub_msg_source_impl::readloop()
    {
      while(!d_finished){

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

          message_port_pub(d_port, m);
        } else {
          boost::this_thread::sleep(boost::posix_time::microseconds(100));
        }
      }
    }

  } /* namespace zeromq */
} /* namespace gr */
