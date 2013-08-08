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

#ifndef INCLUDED_GR_MESSAGE_SOURCE_IMPL_H
#define INCLUDED_GR_MESSAGE_SOURCE_IMPL_H

#include <gnuradio/blocks/message_source.h>
#include <gnuradio/message.h>

namespace gr {
  namespace blocks {

    class message_source_impl : public message_source
    {
    private:
      size_t	 	d_itemsize;
      msg_queue::sptr	d_msgq;
      message::sptr	d_msg;
      unsigned		d_msg_offset;
      bool		d_eof;
      bool              d_tags;
      // FIXME: Is this adequate tagname length.
      std::string       d_lengthtagname;

    public:
      message_source_impl(size_t itemsize, int msgq_limit);
      message_source_impl(size_t itemsize, msg_queue::sptr msgq);
      message_source_impl(size_t itemsize, msg_queue::sptr msgq,
			  const std::string& lengthtagname);

      ~message_source_impl();

      msg_queue::sptr msgq() const { return d_msgq; }

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_SOURCE_IMPL_H */
