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
#include "pull_msg_source_impl.h"

namespace gr {
  namespace zeromq {

    pull_msg_source::sptr
    pull_msg_source::make(char *address, int timeout)
    {
      std::cout << "in make" << std::endl;
      return gnuradio::get_initial_sptr
        (new pull_msg_source_impl(address, timeout));
    }

    pull_msg_source_impl::pull_msg_source_impl(char *address, int timeout)
      : msg_base("pull_msg_source",
                  gr::io_signature::make(0, 0, 0),
                  gr::io_signature::make(0, 0, 0)),
        msg_base_source_impl(ZMQ_PULL, address, timeout)
    {
    }

  } /* namespace zeromq */
} /* namespace gr */
