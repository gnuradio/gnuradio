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
#include "req_msg_source_impl.h"

namespace gr {
  namespace zeromq {

    req_msg_source::sptr
    req_msg_source::make(char *address, int timeout)
    {
      return gnuradio::get_initial_sptr
        (new req_msg_source_impl(address, timeout));
    }

    req_msg_source_impl::req_msg_source_impl(char *address, int timeout)
      : msg_base("req_msg_source",
                  gr::io_signature::make(0, 0, 0),
                  gr::io_signature::make(0, 0, 0)),
        msg_base_source_impl(ZMQ_REQ, address, timeout)
    {
      int time = 0;
      d_socket->setsockopt(ZMQ_LINGER, &time, sizeof(time));
    }

    bool req_msg_source_impl::start()
    {
      d_zmq_finished = false;
      d_thread = new boost::thread(boost::bind(&req_msg_source_impl::readloop, this));
      return true;
    }

    bool req_msg_source_impl::stop()
    {
      d_zmq_finished = true;
      d_thread->join();
      return true;
    }

    void req_msg_source_impl::readloop()
    {
      while(!d_zmq_finished){
        zmq::pollitem_t itemsout[] = { { static_cast<void *>(*d_socket), 0, ZMQ_POLLOUT, 0 } };
        zmq::poll(&itemsout[0], 1, d_timeout);

        //  If we got a reply, process
        if (itemsout[0].revents & ZMQ_POLLOUT) {
          // Request data, FIXME non portable?
          int nmsg = 1;
          zmq::message_t request(sizeof(int));
          memcpy((void *) request.data (), &nmsg, sizeof(int));
          d_socket->send(request);
        }

        zmq::pollitem_t items[] = { { static_cast<void *>(*d_socket), 0, ZMQ_POLLIN, 0 } };
        zmq::poll (&items[0], 1, d_timeout);

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

  } /* namespace zeromq */
} /* namespace gr */
