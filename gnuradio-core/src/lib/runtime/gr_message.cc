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
#include <gr_message.h>
#include <assert.h>
#include <string.h>

static long s_ncurrently_allocated = 0;

gr_message_sptr
gr_make_message (long type, double arg1, double arg2, size_t length)
{
  return gr_message_sptr (new gr_message (type, arg1, arg2, length));
}

gr_message_sptr
gr_make_message_from_string(const std::string s, long type, double arg1, double arg2)
{
  gr_message_sptr m = gr_make_message(type, arg1, arg2, s.size());
  memcpy(m->msg(), s.data(), s.size());
  return m;
}


gr_message::gr_message (long type, double arg1, double arg2, size_t length)
  : d_type(type), d_arg1(arg1), d_arg2(arg2)
{
  if (length == 0)
    d_buf_start = d_msg_start = d_msg_end = d_buf_end = 0;
  else {
    d_buf_start = new unsigned char [length];
    d_msg_start = d_buf_start;
    d_msg_end = d_buf_end = d_buf_start + length;
  }
  s_ncurrently_allocated++;
}

gr_message::~gr_message ()
{
  assert (d_next == 0);
  delete [] d_buf_start;
  d_msg_start = d_msg_end = d_buf_end = 0;
  s_ncurrently_allocated--;
}

std::string
gr_message::to_string() const
{ 
  return std::string((char *)d_msg_start, length());
}

long
gr_message_ncurrently_allocated ()
{
  return s_ncurrently_allocated;
}
