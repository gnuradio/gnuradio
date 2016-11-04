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
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "sub_msg_source_impl.h"
#include "tag_headers.h"

namespace gr {
  namespace zeromq {

    sub_msg_source::sptr
    sub_msg_source::make(char *address, int timeout)
    {
      return gnuradio::get_initial_sptr
        (new sub_msg_source_impl(address, timeout));
    }

    sub_msg_source_impl::sub_msg_source_impl(char *address, int timeout)
      : msg_base("sub_msg_source",
                  gr::io_signature::make(0, 0, 0),
                  gr::io_signature::make(0, 0, 0)),
        msg_base_source_impl(ZMQ_SUB, address, timeout)
    {
      d_socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    }

  } /* namespace zeromq */
} /* namespace gr */
