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
#include "push_msg_sink_impl.h"
#include "tag_headers.h"

namespace gr {
  namespace zeromq {

    push_msg_sink::sptr
    push_msg_sink::make(char *address, int timeout)
    {
      return gnuradio::get_initial_sptr
        (new push_msg_sink_impl(address, timeout));
    }

    push_msg_sink_impl::push_msg_sink_impl(char *address, int timeout)
      : gr::sync_block("push_msg_sink",
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
      d_socket = new zmq::socket_t(*d_context, ZMQ_PUSH);
      int time = 0;
      d_socket->setsockopt(ZMQ_LINGER, &time, sizeof(time));
      d_socket->bind (address);

      message_port_register_in(pmt::mp("in"));
      set_msg_handler( pmt::mp("in"), 
        boost::bind(&push_msg_sink_impl::handler, this, _1));
    }

    push_msg_sink_impl::~push_msg_sink_impl()
    {
      d_socket->close();
      delete d_socket;
      delete d_context;
    }

    void push_msg_sink_impl::handler(pmt::pmt_t msg){
      std::stringbuf sb("");
      pmt::serialize( msg, sb );
      std::string s = sb.str();
      zmq::message_t zmsg(s.size());
      memcpy( zmsg.data(), s.c_str(), s.size() );
      d_socket->send(zmsg);
    }

    int
    push_msg_sink_impl::work(int noutput_items,
                         gr_vector_const_void_star &input_items,
                         gr_vector_void_star &output_items)
    {
      return noutput_items;

/*      const char *in = (const char *) input_items[0];

      zmq::pollitem_t itemsout[] = { { *d_socket, 0, ZMQ_POLLOUT, 0 } };
      zmq::poll (&itemsout[0], 1, d_timeout);

      //  If we got a reply, process
      if (itemsout[0].revents & ZMQ_POLLOUT) {

      // encode the current offset, # tags, and tags into header
      std::string header("");
    
      if(d_pass_tags){
        uint64_t offset = nitems_read(0);
        std::vector<gr::tag_t> tags;
        get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+noutput_items);
        header = gen_tag_header( offset, tags );
        }

      // create message copy and send
      zmq::message_t msg(header.length() + d_itemsize*d_vlen*noutput_items);
      if(d_pass_tags)
        memcpy((void*) msg.data(), header.c_str(), header.length() );
      memcpy((uint8_t *)msg.data() + header.length(), in, d_itemsize*d_vlen*noutput_items);
      d_socket->send(msg);

        return noutput_items;
      }
      else {
        return 0;
      }*/
    }

  } /* namespace zeromq */
} /* namespace gr */
