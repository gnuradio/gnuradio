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
#include "rep_sink_impl.h"

namespace gr {
  namespace zeromq {

    rep_sink::sptr
    rep_sink::make(size_t itemsize, size_t vlen, char *address, int timeout)
    {
      return gnuradio::get_initial_sptr
        (new rep_sink_impl(itemsize, vlen, address, timeout));
    }

    rep_sink_impl::rep_sink_impl(size_t itemsize, size_t vlen, char *address, int timeout)
      : gr::sync_block("rep_sink",
                       gr::io_signature::make(1, 1, itemsize * vlen),
                       gr::io_signature::make(0, 0, 0)),
        d_itemsize(itemsize), d_vlen(vlen), d_timeout(timeout)
    {
      int major, minor, patch;
      zmq::version (&major, &minor, &patch);
      if (major < 3) {
        d_timeout = timeout*1000;
      }
      d_context = new zmq::context_t(1);
      d_socket = new zmq::socket_t(*d_context, ZMQ_REP);
      int time = 0;
      d_socket->setsockopt(ZMQ_LINGER, &time, sizeof(time));
      d_socket->bind (address);
    }

    rep_sink_impl::~rep_sink_impl()
    {
      d_socket->close();
      delete d_socket;
      delete d_context;
    }

    int
    rep_sink_impl::work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];

      zmq::pollitem_t items[] = { { *d_socket, 0, ZMQ_POLLIN, 0 } };
      zmq::poll (&items[0], 1, d_timeout);

      //  If we got a reply, process
      if (items[0].revents & ZMQ_POLLIN) {
        // receive data request
        zmq::message_t request;
        d_socket->recv(&request);
        int req_output_items = *(static_cast<int*>(request.data()));

        // create message copy and send
        if (noutput_items < req_output_items) {
          zmq::message_t msg(d_itemsize*d_vlen*noutput_items);
          memcpy((void *)msg.data(), in, d_itemsize*d_vlen*noutput_items);
          d_socket->send(msg);

          return noutput_items;
        }
        else {
          zmq::message_t msg(d_itemsize*d_vlen*req_output_items);
          memcpy((void *)msg.data(), in, d_itemsize*d_vlen*req_output_items);
          d_socket->send(msg);

          return req_output_items;
        }
      }

      return 0;
    }
  } /* namespace zeromq */
} /* namespace gr */
