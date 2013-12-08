/* -*- c++ -*- */
/* 
 * Copyright 2013 Free Software Foundation, Inc.
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
#include "source_pushpull_impl.h"

namespace gr {
  namespace zeromq {

    source_pushpull::sptr
    source_pushpull::make(size_t itemsize, char *address)
    {
      return gnuradio::get_initial_sptr
        (new source_pushpull_impl(itemsize, address));
    }

    /*
     * The private constructor
     */
    source_pushpull_impl::source_pushpull_impl(size_t itemsize, char *address)
      : gr::sync_block("source_pushpull",
                       gr::io_signature::make(0, 0, 0),
                       gr::io_signature::make(1, 1, itemsize)),
        d_itemsize(itemsize)
    {
        d_context = new zmq::context_t(1);
        d_socket = new zmq::socket_t(*d_context, ZMQ_PULL);
        d_socket->connect (address);
        std::cout << "source_pushpull on " << address << std::endl;
    }

    /*
     * Our virtual destructor.
     */
    source_pushpull_impl::~source_pushpull_impl()
    {
        delete(d_socket);
        delete(d_context);
    }

    int
    source_pushpull_impl::work(int noutput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items)
    {
        char *out = (char*)output_items[0];

        zmq::pollitem_t items[] = { { *d_socket, 0, ZMQ_POLLIN, 0 } };
        zmq::poll (&items[0], 1, 1000);
        //  If we got a reply, process
        if (items[0].revents & ZMQ_POLLIN) {

            // Receive data
            zmq::message_t msg;
            d_socket->recv(&msg);

            // Copy to ouput buffer and return
            if (msg.size() >= d_itemsize*noutput_items) {
                memcpy(out, (void *)msg.data(), d_itemsize*noutput_items);
                return noutput_items;
            } else {
                memcpy(out, (void *)msg.data(), msg.size());
                return msg.size()/d_itemsize;
            }
        } else {
            return 0;
        }
    }

  } /* namespace zeromq */
} /* namespace gr */

