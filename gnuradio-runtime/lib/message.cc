/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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

#include <gnuradio/message.h>
#include <assert.h>
#include <string.h>

namespace gr {

  static long s_ncurrently_allocated = 0;

  message::sptr
  message::make(long type, double arg1, double arg2, size_t length)
  {
    return message::sptr(new message(type, arg1, arg2, length));
  }

  message::sptr
  message::make_from_string(const std::string s, long type, double arg1, double arg2)
  {
    message::sptr m = message::make(type, arg1, arg2, s.size());
    memcpy(m->msg(), s.data(), s.size());
    return m;
  }

  message::message(long type, double arg1, double arg2, size_t length)
    : d_type(type), d_arg1(arg1), d_arg2(arg2)
  {
    if(length == 0)
      d_buf_start = d_msg_start = d_msg_end = d_buf_end = 0;
    else {
      d_buf_start = new unsigned char[length];
      d_msg_start = d_buf_start;
      d_msg_end = d_buf_end = d_buf_start + length;
    }
    s_ncurrently_allocated++;
  }

  message::~message()
  {
    assert (d_next == 0);
    delete [] d_buf_start;
    d_msg_start = d_msg_end = d_buf_end = 0;
    s_ncurrently_allocated--;
  }

  std::string
  message::to_string() const
  {
    return std::string((char *)d_msg_start, length());
  }

  long
  message_ncurrently_allocated()
  {
    return s_ncurrently_allocated;
  }

} /* namespace gr */
