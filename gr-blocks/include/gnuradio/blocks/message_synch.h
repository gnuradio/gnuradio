/* -*- c++ -*- */
/*
 * Copyright 2005,2012-2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_MESSAGE_SYNCH_H
#define INCLUDED_GR_MESSAGE_SYNCH_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Synchronize block for the message passing system.
     * \ingroup message_tools_blk
     *
     * \details
     * The message debug block is used to allow fan-in synchronization
     * of 1 message on many ports to a tuple of syncrhonized 
     * messages on a single output port
     *
     * \li in: input ports to synchronize
     * \li out: an N-tuple once a message has arrived on each of N input ports
     */
    class BLOCKS_API message_synch : virtual public block
    {
    public:
      // gr::blocks::message_synch::sptr
      typedef boost::shared_ptr<message_synch> sptr;

      /*!
       * \brief Build the message synch block. It takes one parameter
       * the number of input ports/messages to synchronize into a 
       * single output tuple
       */
      static sptr make(int n_inputs);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_SYNCH_H */
