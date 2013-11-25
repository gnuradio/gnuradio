/* -*- c++ -*- */
/* 
 * Copyright 2013 Institute for Theoretical Information Technology,
 *                RWTH Aachen University
 * 
 * Authors: Johannes Schmitz <schmitz@ti.rwth-aachen.de>
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
#include "source_reqrep_nopoll_impl.h"
#include <cstring>

namespace gr {
  namespace zmqblocks {

    source_reqrep_nopoll::sptr
    source_reqrep_nopoll::make(size_t itemsize, char *address)
    {
      return gnuradio::get_initial_sptr
        (new source_reqrep_nopoll_impl(itemsize, address));
    }

    /*
     * The private constructor
     */
    source_reqrep_nopoll_impl::source_reqrep_nopoll_impl(size_t itemsize, char *address)
      : gr::sync_block("source_reqrep_nopoll",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, itemsize)),
        d_itemsize(itemsize)
    {
        d_context = new zmq::context_t(1);
        d_socket = new zmq::socket_t(*d_context, ZMQ_REQ);
        d_socket->connect (address);
        std::cout << "source_reqrep_nopoll on " << address << std::endl;
    }

    /*
     * Our virtual destructor.
     */
    source_reqrep_nopoll_impl::~source_reqrep_nopoll_impl()
    {
        delete(d_socket);
        delete(d_context);
    }

    int
    source_reqrep_nopoll_impl::work(int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
    {
        char *out = (char*)output_items[0];

        // Request data, FIXME non portable
        zmq::message_t request(sizeof(int));
        memcpy ((void *) request.data (), &noutput_items, sizeof(int));
        d_socket->send(request);

        // Receive data
        zmq::message_t reply;
        d_socket->recv(&reply);

        // Copy to ouput buffer and return
        memcpy(out, (void *)reply.data(), reply.size());
        return reply.size()/d_itemsize;
    }

  } /* namespace zmqblocks */
} /* namespace gr */

