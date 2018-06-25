/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
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
#include "base_impl.h"
#include "tag_headers.h"

namespace gr {
  namespace zeromq {

    base_impl::base_impl(int type, size_t itemsize, size_t vlen, int timeout, bool pass_tags)
      : d_vsize(itemsize * vlen), d_timeout(timeout), d_pass_tags(pass_tags)
    {
      /* "Fix" timeout value (ms for new API, us for old API) */
      int major, minor, patch;
      zmq::version (&major, &minor, &patch);

      if (major < 3) {
        d_timeout *= 1000;
      }

      /* Create context & socket */
      d_context = new zmq::context_t(1);
      d_socket = new zmq::socket_t(*d_context, type);
    }

    base_impl::~base_impl()
    {
        d_socket->close();
        delete d_socket;
        delete d_context;
    }


    base_sink_impl::base_sink_impl(int type, size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags, int hwm)
        : base_impl(type, itemsize, vlen, timeout, pass_tags)
    {
      /* Set high watermark */
      if (hwm >= 0) {
#ifdef ZMQ_SNDHWM
        d_socket->setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));
#else // major < 3
        uint64_t tmp = hwm;
        d_socket->setsockopt(ZMQ_HWM, &tmp, sizeof(tmp));
#endif
      }

      /* Bind */
      d_socket->bind(address);
    }

    int
    base_sink_impl::send_message(const void *in_buf, const int in_nitems, const uint64_t in_offset)
    {
      /* Meta-data header */
      std::string header("");
      if(d_pass_tags){
        std::vector<gr::tag_t> tags;
        get_tags_in_range(tags, 0, in_offset, in_offset + in_nitems);
        header = gen_tag_header(in_offset, tags);
      }

      /* Create message */
      size_t payload_len = in_nitems * d_vsize;
      size_t msg_len = d_pass_tags ? payload_len + header.length() : payload_len;
      zmq::message_t msg(msg_len);

      if(d_pass_tags){
        memcpy(msg.data(), header.c_str(), header.length());
        memcpy((uint8_t*)msg.data() + header.length(), in_buf, payload_len);
      } else {
        memcpy(msg.data(), in_buf, payload_len);
      }

      /* Send */
      d_socket->send(msg);

      /* Report back */
      return in_nitems;
    }

    base_source_impl::base_source_impl(int type, size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags, int hwm)
        : base_impl(type, itemsize, vlen, timeout, pass_tags),
          d_consumed_bytes(0), d_consumed_items(0)
    {
      /* Set high watermark */
      if (hwm >= 0) {
#ifdef ZMQ_RCVHWM
        d_socket->setsockopt(ZMQ_RCVHWM, &hwm, sizeof(hwm));
#else // major < 3
        uint64_t tmp = hwm;
        d_socket->setsockopt(ZMQ_HWM, &tmp, sizeof(tmp));
#endif
      }

      /* Connect */
      d_socket->connect(address);
    }

    bool
    base_source_impl::has_pending()
    {
      return d_msg.size() > d_consumed_bytes;
    }

    int
    base_source_impl::flush_pending(void *out_buf, const int out_nitems, const uint64_t out_offset)
    {
      /* How much to copy in this call */
      int to_copy_items = std::min(out_nitems, (int)((d_msg.size() - d_consumed_bytes) / d_vsize));
      int to_copy_bytes = d_vsize * to_copy_items;

      /* Copy actual data */
      memcpy(out_buf, (uint8_t*)d_msg.data() + d_consumed_bytes, to_copy_bytes);

      /* Add tags matching this segment of samples */
      for (unsigned int i=0; i<d_tags.size(); i++)
      {
        if ((d_tags[i].offset >= (uint64_t)d_consumed_items) &&
            (d_tags[i].offset  < (uint64_t)d_consumed_items + to_copy_items))
        {
          gr::tag_t nt = d_tags[i];
          nt.offset += out_offset - d_consumed_items;
          add_item_tag(0, nt);
        }
      }

      /* Update pointer */
      d_consumed_items += to_copy_items;
      d_consumed_bytes += to_copy_bytes;

      return to_copy_items;
    }

    bool
    base_source_impl::load_message(bool wait)
    {
      /* Poll for input */
      zmq::pollitem_t items[] = { { static_cast<void *>(*d_socket), 0, ZMQ_POLLIN, 0 } };
      zmq::poll(&items[0], 1, wait ? d_timeout : 0);

      if (!(items[0].revents & ZMQ_POLLIN))
        return false;

      /* Is this the start or continuation of a multi-part message? */
      int64_t more = 0;
      size_t more_len = sizeof(more);
      d_socket->getsockopt(ZMQ_RCVMORE, &more, &more_len);

      /* Reset */
      d_msg.rebuild();
      d_tags.clear();
      d_consumed_items = 0;
      d_consumed_bytes = 0;

      /* Get the message */
      d_socket->recv(&d_msg);

      /* Parse header from the first (or only) message of a multi-part message */
      if (d_pass_tags && !more)
      {
        uint64_t rcv_offset;

        /* Parse header */
        d_consumed_bytes = parse_tag_header(d_msg, rcv_offset, d_tags);

        /* Fixup the tags offset to be relative to the start of this message */
        for (unsigned int i=0; i<d_tags.size(); i++) {
          d_tags[i].offset -= rcv_offset;
        }
      }

      /* Each message must contain an integer multiple of data vectors */
      if ((d_msg.size() - d_consumed_bytes) % d_vsize != 0)
      {
        throw std::runtime_error(
            boost::str(boost::format("Incompatible vector sizes: "
                                     "need a multiple of %1% bytes per message") % d_vsize));
      }

      /* We got one ! */
      return true;
    }

  } /* namespace zeromq */
} /* namespace gr */

// vim: ts=2 sw=2 expandtab
