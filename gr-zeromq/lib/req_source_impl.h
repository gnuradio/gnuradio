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

#ifndef INCLUDED_ZEROMQ_REQ_SOURCE_IMPL_H
#define INCLUDED_ZEROMQ_REQ_SOURCE_IMPL_H

#include <gnuradio/zeromq/req_source.h>
#include <zmq.hpp>

namespace gr {
  namespace zeromq {

    class req_source_impl : public req_source
    {
    private:
      size_t          d_itemsize;
      size_t          d_vlen;
      int             d_timeout;
      zmq::context_t  *d_context;
      zmq::socket_t   *d_socket;
      bool            d_pass_tags;

    public:
      req_source_impl(size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags);
      ~req_source_impl();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_REQ_SOURCE_IMPL_H */
