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

#ifndef INCLUDED_ZEROMQ_BASE_IMPL_H
#define INCLUDED_ZEROMQ_BASE_IMPL_H

#include <zmq.hpp>

#include <gnuradio/sync_block.h>

namespace gr {
  namespace zeromq {

    class base_impl : public virtual gr::sync_block
    {
    public:
      base_impl(int type, size_t itemsize, size_t vlen, int timeout, bool pass_tags);
      virtual ~base_impl();

    protected:
      zmq::context_t  *d_context;
      zmq::socket_t   *d_socket;
      size_t          d_vsize;
      int             d_timeout;
      bool            d_pass_tags;
    };

    class base_sink_impl : public base_impl
    {
    public:
      base_sink_impl(int type, size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags, int hwm);

    protected:
      int send_message(const void *in_buf, const int in_nitems, const uint64_t in_offset);
    };

    class base_source_impl : public base_impl
    {
    public:
      base_source_impl(int type, size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags, int hwm);

    protected:
      zmq::message_t d_msg;
      std::vector<gr::tag_t> d_tags;
      size_t d_consumed_bytes;
      int    d_consumed_items;

      bool has_pending();
      int  flush_pending(void *out_buf, const int out_nitems, const uint64_t out_offset);
      bool load_message(bool wait);
    };

  } // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_BASE_IMPL_H */

// vim: ts=2 sw=2 expandtab
