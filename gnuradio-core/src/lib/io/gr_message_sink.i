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

GR_SWIG_BLOCK_MAGIC(gr,message_sink);

#include <string>

gr_message_sink_sptr gr_make_message_sink (size_t itemsize,
					   gr_msg_queue_sptr msgq,
					   bool dont_block);
gr_message_sink_sptr gr_make_message_sink (size_t itemsize,
                                           gr_msg_queue_sptr msgq,
                                           bool dont_block,
                                           const std::string& lengthtagname);

class gr_message_sink : public gr_sync_block
{
 protected:
  gr_message_sink (size_t itemsize, gr_msg_queue_sptr msgq, bool dont_block);
  gr_message_sink (size_t itemsize, gr_msg_queue_sptr msgq, bool dont_block, const std::string& lengthtagname);

 public:
  ~gr_message_sink ();
};
