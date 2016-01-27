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
    pub_sink::make(size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags, int hwm)
    {
      return gnuradio::get_initial_sptr
        (new pub_sink_impl(itemsize, vlen, address, timeout, pass_tags, hwm));
    }

    pub_sink_impl::pub_sink_impl(size_t itemsize, size_t vlen, char *address, int timeout, bool pass_tags, int hwm)
      : gr::sync_block("pub_sink",
                       gr::io_signature::make(1, 1, itemsize * vlen),
                       gr::io_signature::make(0, 0, 0)),
        base_sink_impl(ZMQ_PUB, itemsize, vlen, address, timeout, pass_tags, hwm)
    {
      /* All is delegated */
    }

    int
    pub_sink_impl::work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
    {
      return send_message(input_items[0], noutput_items, nitems_read(0));
    }

  } /* namespace zeromq */
} /* namespace gr */

// vim: ts=2 sw=2 expandtab
