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
#include "rep_msg_sink_impl.h"
#include "tag_headers.h"

namespace gr {
  namespace zeromq {

    rep_msg_sink::sptr
    rep_msg_sink::make(char *address, int timeout)
    {
      return gnuradio::get_initial_sptr
        (new rep_msg_sink_impl(address, timeout));
    }

    rep_msg_sink_impl::rep_msg_sink_impl(char *address, int timeout)
      : gr::block("rep_msg_sink",
                  gr::io_signature::make(0, 0, 0),
                  gr::io_signature::make(0, 0, 0)),
      d_timeout(timeout),
      d_port(pmt::mp("in"))
    {
      int major, minor, patch;
      zmq::version(&major, &minor, &patch);

      if (major < 3) {
        d_timeout = timeout*1000;
      }

      d_context = new zmq::context_t(1);
      d_socket = new zmq::socket_t(*d_context, ZMQ_REP);

      int time = 0;
      d_socket->setsockopt(ZMQ_LINGER, &time, sizeof(time));
      d_socket->bind (address);

      message_port_register_in(d_port);
    }

    rep_msg_sink_impl::~rep_msg_sink_impl()
    {
      d_socket->close();
      delete d_socket;
      delete d_context;
    }

    bool rep_msg_sink_impl::start()
    {
      d_finished = false;
      d_thread = new boost::thread(boost::bind(&rep_msg_sink_impl::readloop, this));
      return true;
    }

    bool rep_msg_sink_impl::stop()
    {
      d_finished = true;
      d_thread->join();
      return true;
    }

    void rep_msg_sink_impl::readloop()
    {
      while(!d_finished) {

        // while we have data, wait for query...
        while(!empty_p(d_port)) {

          // wait for query...
          zmq::pollitem_t items[] = { { static_cast<void *>(*d_socket), 0, ZMQ_POLLIN, 0 } };
          zmq::poll (&items[0], 1, d_timeout);

          //  If we got a reply, process
          if (items[0].revents & ZMQ_POLLIN) {

            // receive data request
            zmq::message_t request;
            d_socket->recv(&request);

            int req_output_items = *(static_cast<int*>(request.data()));
            if(req_output_items != 1)
              throw std::runtime_error("Request was not 1 msg for rep/req request!!");

            // create message copy and send
            pmt::pmt_t msg = delete_head_nowait(d_port);
            std::stringbuf sb("");
            pmt::serialize( msg, sb );
            std::string s = sb.str();
            zmq::message_t zmsg(s.size());
            memcpy( zmsg.data(), s.c_str(), s.size() );
            d_socket->send(zmsg);
          } // if req
        } // while !empty

      } // while !d_finished
    }

  } /* namespace zeromq */
} /* namespace gr */
