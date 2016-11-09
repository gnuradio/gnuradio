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
      : msg_base("rep_msg_sink",
                  gr::io_signature::make(0, 0, 0),
                  gr::io_signature::make(0, 0, 0)),
        msg_base_sink_impl(ZMQ_REP, address, timeout)
    {
    }

    bool rep_msg_sink_impl::start()
    {
      d_zmq_finished = false;
      d_thread = new boost::thread(boost::bind(&rep_msg_sink_impl::readloop, this));
      return true;
    }

    bool rep_msg_sink_impl::stop()
    {
      if (d_zmq_started) {
        d_zmq_finished = true;
        d_thread->join();
      }
      return true;
    }

    void rep_msg_sink_impl::readloop()
    {
      gr::thread::scoped_lock guard(d_mutex);
      while(!d_zmq_finished) {
        // while we have data, wait for query...
        while(!empty_p(pmt::mp("in"))) {

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
            pmt::pmt_t msg = delete_head_nowait(pmt::mp("in"));
            std::stringbuf sb("");
            pmt::serialize( msg, sb );
            std::string s = sb.str();
            zmq::message_t zmsg(s.size());
            memcpy( zmsg.data(), s.c_str(), s.size() );
            d_socket->send(zmsg);
          } // if req
        } // while !empty

      } // while !d_zmq_finished
    }

  } /* namespace zeromq */
} /* namespace gr */
