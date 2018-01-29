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
#include "tag_headers.h"

namespace gr {
  namespace zeromq {

    rep_sink::sptr
    rep_sink::make(size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags, int hwm)
    {
      return gnuradio::get_initial_sptr
        (new rep_sink_impl(itemsize, vlen, address, timeout, pass_tags, hwm));
    }

    rep_sink_impl::rep_sink_impl(size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags, int hwm)
      : gr::sync_block("rep_sink",
                       gr::io_signature::make(1, 1, itemsize * vlen),
                       gr::io_signature::make(0, 0, 0)),
        base_sink_impl(ZMQ_REP, itemsize, vlen, address, timeout, pass_tags, hwm)
    {
      /* All is delegated */
    }

    int
    rep_sink_impl::work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      bool first = true;
      int done = 0;

      /* Process as much as we can */
      while (1)
      {
        /* Wait for a small time (FIXME: scheduler can't wait for us) */
          /* We only wait if its the first iteration, for the others we'll
           * let the scheduler retry */
        zmq::pollitem_t items[] = { { static_cast<void *>(*d_socket), 0, ZMQ_POLLIN, 0 } };
        zmq::poll(&items[0], 1, first ? d_timeout : 0);

          /* If we don't have anything, we're done */
        if (!(items[0].revents & ZMQ_POLLIN))
          break;

        /* Get and parse the request */
        zmq::message_t request;
        d_socket->recv(&request);

        int nitems_send = noutput_items - done;
        if (request.size() >= sizeof(uint32_t))
        {
          int req = (int)*(static_cast<uint32_t*>(request.data()));
          nitems_send = std::min(nitems_send, req);
        }

        /* Delegate the actual send */
        done += send_message(in + (done * d_vsize), nitems_send, nitems_read(0) + done);

        /* Not the first anymore */
        first = false;
      }

      return done;
    }
  } /* namespace zeromq */
} /* namespace gr */

// vim: ts=2 sw=2 expandtab
