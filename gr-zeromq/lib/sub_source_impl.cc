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
#include "sub_source_impl.h"

namespace gr {
  namespace zeromq {

    sub_source::sptr
    sub_source::make(size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags)
    {
      return gnuradio::get_initial_sptr
        (new sub_source_impl(itemsize, vlen, address, timeout));
    }

    sub_source_impl::sub_source_impl(size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags)
      : gr::sync_block("sub_source",
                       gr::io_signature::make(0, 0, 0),
                       gr::io_signature::make(1, 1, itemsize * vlen)),
        d_itemsize(itemsize), d_vlen(vlen), d_timeout(timeout), d_pass_tags(pass_tags)
    {
      int major, minor, patch;
      zmq::version (&major, &minor, &patch);
      if (major < 3) {
        d_timeout = timeout*1000;
      }
      d_context = new zmq::context_t(1);
      d_socket = new zmq::socket_t(*d_context, ZMQ_SUB);
      //int time = 0;
      d_socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
      d_socket->connect (address);
    }

    /*
     * Our virtual destructor.
     */
    sub_source_impl::~sub_source_impl()
    {
      d_socket->close();
      delete d_socket;
      delete d_context;
    }

    int
    sub_source_impl::work(int noutput_items,
                           gr_vector_const_void_star &input_items,
                           gr_vector_void_star &output_items)
    {
      char *out = (char*)output_items[0];

      zmq::pollitem_t items[] = { { *d_socket, 0, ZMQ_POLLIN, 0 } };
      zmq::poll (&items[0], 1, d_timeout);

      //  If we got a reply, process
      if (items[0].revents & ZMQ_POLLIN) {

        // Receive data
        zmq::message_t msg;
        d_socket->recv(&msg);

        // Deserialize header data / tags
        std::istringstream iss( std::string(static_cast<char*>(msg.data()), msg.size()));

        if(d_pass_tags){
        uint64_t rcv_offset;
        size_t   rcv_ntags;
        iss.read( (char*)&rcv_offset, sizeof(uint64_t ) );
        iss.read( (char*)&rcv_ntags,  sizeof(size_t   ) );
        for(size_t i=0; i<rcv_ntags; i++){
            uint64_t tag_offset;
            iss.read( (char*)&tag_offset, sizeof(uint64_t ) );
            std::stringbuf sb( iss.str() );
            pmt::pmt_t key = pmt::deserialize( sb );
            pmt::pmt_t val = pmt::deserialize( sb );
            pmt::pmt_t src = pmt::deserialize( sb );
            uint64_t new_tag_offset = tag_offset + nitems_read(0) - rcv_offset;
            add_item_tag(0, new_tag_offset, key, val, src);
            iss.str(sb.str());
            }
        }

        // Pass sample data along
        std::vector<char> samp(iss.gcount());
        iss.read( &samp[0], iss.gcount() );

        // Copy to ouput buffer and return
        if (samp.size() >= d_itemsize*d_vlen*noutput_items) {
          memcpy(out, (void *)&samp[0], d_itemsize*d_vlen*noutput_items);
          return noutput_items;
        }
        else {
          memcpy(out, (void *)&samp[0], samp.size());
          return samp.size()/(d_itemsize*d_vlen);
        }
      }
      else {
        return 0; // FIXME: someday when the scheduler does all the poll/selects
      }
    }

  } /* namespace zeromq */
} /* namespace gr */
