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
#include "req_source_impl.h"
#include "tag_headers.h"

namespace gr {
  namespace zeromq {

    req_source::sptr
    req_source::make(size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags, int hwm)
    {
      return gnuradio::get_initial_sptr
        (new req_source_impl(itemsize, vlen, address, timeout, pass_tags, hwm));
    }

    req_source_impl::req_source_impl(size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags, int hwm)
      : gr::sync_block("req_source",
                       gr::io_signature::make(0, 0, 0),
                       gr::io_signature::make(1, 1, itemsize * vlen)),
        base_source_impl(ZMQ_REQ, itemsize, vlen, address, timeout, pass_tags, hwm),
        d_req_pending(false)
    {
      /* All is delegated */
    }

    int
    req_source_impl::work(int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
    {
#if 0
#endif
      uint8_t *out = (uint8_t *) output_items[0];
      bool first = true;
      int done = 0;

      /* Process as much as we can */
      while (1)
      {
        if (has_pending())
        {
          /* Flush anything pending */
          done += flush_pending(out + (done * d_vsize), noutput_items - done, nitems_written(0) + done);

          /* No more space ? */
          if (done == noutput_items)
            break;
        }
        else
        {
          /* Send request if needed */
          if (!d_req_pending)
          {
            /* The REP/REQ pattern state machine guarantees we can send at this point */
            uint32_t req_len = noutput_items - done;
            zmq::message_t request(sizeof(uint32_t));
            memcpy ((void *) request.data (), &req_len, sizeof(uint32_t));
            d_socket->send(request);

            d_req_pending = true;
          }

          /* Try to get the next message */
          if (!load_message(first))
            break;  /* No message, we're done for now */

          /* Got response */
          d_req_pending = false;

          /* Not the first anymore */
          first = false;
        }
      }

      return done;

      return 0;
    }

  } /* namespace zeromq */
} /* namespace gr */

// vim: ts=2 sw=2 expandtab
