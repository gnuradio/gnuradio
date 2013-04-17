/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_MESSAGE_SINK_IMPL_H
#define INCLUDED_GR_MESSAGE_SINK_IMPL_H

#include <gnuradio/blocks/message_sink.h>

namespace gr {
  namespace blocks {

    class message_sink_impl : public message_sink
    {
    private:
      size_t	 	d_itemsize;
      msg_queue::sptr	d_msgq;
      bool		d_dont_block;
      bool              d_tags;
      std::string       d_lengthtagname;
      uint64_t          d_items_read;

    public:
      message_sink_impl(size_t itemsize, msg_queue::sptr msgq, bool dont_block);
      message_sink_impl(size_t itemsize, msg_queue::sptr msgq, bool dont_block,
			const std::string& lengthtagname);

      ~message_sink_impl();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_SINK_IMPL_H */
