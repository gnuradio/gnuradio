/* -*- c++ -*- */
/*
 * Copyright 2013,2014,2016 Free Software Foundation, Inc.
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
#include "pub_msg_sink_impl.h"

namespace gr {
  namespace zeromq {

    pub_msg_sink::sptr
    pub_msg_sink::make(char *address, int timeout)
    {
      return gnuradio::get_initial_sptr
        (new pub_msg_sink_impl(address, timeout));
    }

    pub_msg_sink_impl::pub_msg_sink_impl(char *address, int timeout)
      : msg_base("pub_msg_sink",
                  gr::io_signature::make(0, 0, 0),
                  gr::io_signature::make(0, 0, 0)),
        msg_base_sink_impl(ZMQ_PUB, address, timeout)
    {
    }

  } /* namespace zeromq */
} /* namespace gr */
