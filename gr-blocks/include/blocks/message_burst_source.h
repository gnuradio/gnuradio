/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_MESSAGE_BURST_SOURCE_H
#define INCLUDED_GR_MESSAGE_BURST_SOURCE_H

#include <blocks/api.h>
#include <gr_sync_block.h>
#include <gr_msg_queue.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Turn received messages into a stream and tag them for UHD to send.
     * \ingroup source_blk
     */
    class BLOCKS_API message_burst_source : virtual public gr_sync_block
    {
    public:
      // gr::blocks::message_source::sptr
      typedef boost::shared_ptr<message_burst_source> sptr;

      static sptr make(size_t itemsize, int msgq_limit);
      static sptr make(size_t itemsize, gr_msg_queue_sptr msgq);

      virtual gr_msg_queue_sptr msgq() const = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_BURST_SOURCE_H */
