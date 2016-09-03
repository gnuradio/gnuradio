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

#ifndef INCLUDED_GR_MESSAGE_SINK_H
#define INCLUDED_GR_MESSAGE_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/msg_queue.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Gather received items into messages and insert into msgq
     * \ingroup deprecated_blk
     */
    class BLOCKS_API message_sink : virtual public sync_block
    {
    public:
      // gr::blocks::message_sink::sptr
      typedef boost::shared_ptr<message_sink> sptr;

      static sptr make(size_t itemsize, gr::msg_queue::sptr msgq, bool dont_block);
      static sptr make(size_t itemsize, gr::msg_queue::sptr msgq, bool dont_block,
		       const std::string& lengthtagname);

    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_SINK_H */
