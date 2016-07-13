/* -*- c++ -*- */
/* Copyright 2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <string.h>
#include <volk/volk.h>
#include <gnuradio/digital/header_format_base.h>
#include <gnuradio/math.h>

namespace gr {
  namespace digital {

    header_format_base::header_format_base()
    {
      enter_search();
      configure_default_loggers(d_logger, d_debug_logger, "packet formatter");
    }

    header_format_base::~header_format_base()
    {
    }

    size_t
    header_format_base::header_nbytes() const
    {
      return header_nbits() / 8;
    }

    inline void
    header_format_base::enter_search()
    {
      d_state = STATE_SYNC_SEARCH;
    }

    inline void
    header_format_base::enter_have_sync()
    {
      d_state = STATE_HAVE_SYNC;
    }

    inline void
    header_format_base::enter_have_header(int payload_len)
    {
      d_state = STATE_SYNC_SEARCH;
    }

  } /* namespace digital */
} /* namespace gr */
