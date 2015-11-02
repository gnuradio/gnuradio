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
#include "pub_sink_impl.h"
#include "tag_headers.h"

namespace gr {
  namespace zeromq {

    pub_sink::sptr
    pub_sink::make(size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags)
    {
      return gnuradio::get_initial_sptr
        (new pub_sink_impl(itemsize, vlen, address, timeout, pass_tags));
    }

    pub_sink_impl::pub_sink_impl(size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags)
      : gr::sync_block("pub_sink",
                       gr::io_signature::make(1, 1, itemsize * vlen),
                       gr::io_signature::make(0, 0, 0)),
        d_itemsize(itemsize), d_vlen(vlen), d_timeout(timeout), d_pass_tags(pass_tags)
    {
      int major, minor, patch;
      zmq::version (&major, &minor, &patch);
      if (major < 3) {
        d_timeout = timeout*1000;
      }
      d_context = new zmq::context_t(1);
      d_socket = new zmq::socket_t(*d_context, ZMQ_PUB);
      int time = 0;
      d_socket->setsockopt(ZMQ_LINGER, &time, sizeof(time));
      d_socket->bind(address);
    }

    pub_sink_impl::~pub_sink_impl()
    {
      d_socket->close();
      delete d_socket;
      delete d_context;
    }

    int
    pub_sink_impl::work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
    {
      const char *in = (const char *)input_items[0];

      // encode the current offset, # tags, and tags into header
    std::string header("");
    if(d_pass_tags){
      uint64_t offset = nitems_read(0);
      std::vector<gr::tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+noutput_items);
      header = gen_tag_header( offset, tags );  
      }

      // create message copy and send
      int payloadlen = d_itemsize * d_vlen * noutput_items;
      int msglen = d_pass_tags ? payloadlen + header.length() : payloadlen;
      zmq::message_t msg(msglen);

      if(d_pass_tags){
        memcpy((void*) msg.data(), header.c_str(), header.length() );
        memcpy((uint8_t *)msg.data() + header.length(), in, d_itemsize*d_vlen*noutput_items);
        } else {
        memcpy((uint8_t *)msg.data(), in, d_itemsize*d_vlen*noutput_items);
        }
    
      d_socket->send(msg);

      return noutput_items;
    }

  } /* namespace zeromq */
} /* namespace gr */
