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
#include "req_msg_source_impl.h"
#include "tag_headers.h"

namespace gr {
  namespace zeromq {

    req_msg_source::sptr
    req_msg_source::make(char *address, int timeout)
    {
      return gnuradio::get_initial_sptr
        (new req_msg_source_impl(address, timeout));
    }

    req_msg_source_impl::req_msg_source_impl(char *address, int timeout)
      : gr::sync_block("req_msg_source",
                       gr::io_signature::make(0, 0, 0),
                       gr::io_signature::make(0, 0, 0)),
       d_timeout(timeout)
    {
      int major, minor, patch;
      zmq::version (&major, &minor, &patch);
      if (major < 3) {
        d_timeout = timeout*1000;
      }
      d_context = new zmq::context_t(1);
      d_socket = new zmq::socket_t(*d_context, ZMQ_REQ);
      int time = 0;
      d_socket->setsockopt(ZMQ_LINGER, &time, sizeof(time));
      d_socket->connect (address);

      message_port_register_out(pmt::mp("out"));
    }

    req_msg_source_impl::~req_msg_source_impl()
    {
      d_socket->close();
      delete d_socket;
      delete d_context;
    }

    int
    req_msg_source_impl::work(int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
    {
      return noutput_items;
/*
      char *out = (char*)output_items[0];

      zmq::pollitem_t itemsout[] = { { *d_socket, 0, ZMQ_POLLOUT, 0 } };
      zmq::poll (&itemsout[0], 1, d_timeout);

      //  If we got a reply, process
      if (itemsout[0].revents & ZMQ_POLLOUT) {
        // Request data, FIXME non portable?
        zmq::message_t request(sizeof(int));
        memcpy ((void *) request.data (), &noutput_items, sizeof(int));
        d_socket->send(request);
      }

      zmq::pollitem_t itemsin[] = { { *d_socket, 0, ZMQ_POLLIN, 0 } };
      zmq::poll (&itemsin[0], 1, d_timeout);

      //  If we got a reply, process
      if (itemsin[0].revents & ZMQ_POLLIN) {
        // Receive data
        zmq::message_t reply;
        d_socket->recv(&reply);

        // Deserialize header data / tags
        std::string buf(static_cast<char*>(reply.data()), reply.size());

        if(d_pass_tags){
            uint64_t rcv_offset;
            std::vector<gr::tag_t> tags;
            buf = parse_tag_header(buf, rcv_offset, tags);
            for(size_t i=0; i<tags.size(); i++){
                tags[i].offset -= rcv_offset - nitems_written(0);
                add_item_tag(0, tags[i]);
                }
            }


        // Copy to ouput buffer and return
        memcpy(out, (void *)&buf[0], buf.size());
        return buf.size()/(d_itemsize*d_vlen);
      }

      return 0;
        */
    }

  } /* namespace zeromq */
} /* namespace gr */
