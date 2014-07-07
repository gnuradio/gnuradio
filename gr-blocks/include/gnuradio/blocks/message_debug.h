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

#ifndef INCLUDED_GR_MESSAGE_DEBUG_H
#define INCLUDED_GR_MESSAGE_DEBUG_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Debug block for the message passing system.
     * \ingroup message_tools_blk
     * \ingroup measurement_tools_blk
     * \ingroup debug_tools_blk
     *
     * \details
     * The message debug block is used to capture and print or store
     * messages as they are received. Any block that generates a
     * message may connect that message port to one or more of the
     * three message input ports of this debug block. The message
     * ports are:
     *
     * \li print: prints the message directly to standard out.
     * \li store: stores the message in an internal vector. May be
     *            access using the get_message function.
     * \li print_pdu: specifically designed to handle formatted PDUs
     *                (see pdu.h).
     */
    class BLOCKS_API message_debug : virtual public block
    {
    public:
      // gr::blocks::message_debug::sptr
      typedef boost::shared_ptr<message_debug> sptr;

      /*!
       * \brief Build the message debug block. It takes no parameters
       * and has three message ports: print, store, and
       * print_pdu.
       */
      static sptr make();

      /*!
       * \brief Reports the number of messages received by this block.
       */
      virtual int num_messages() = 0;

      /*!
       * \brief Get a message (as a PMT) from the message vector at index \p i.
       *
       * Messages passed to the 'store' port will be stored in a
       * vector. This function retrieves those messages by index. They
       * are index in order of when they were received (all messages
       * are just pushed onto the back of a vector). This is mostly
       * useful in debugging message passing graphs and in QA code.
       *
       * \param i The index in the vector for the message to retrieve.
       *
       * \return a message at index \p i as a pmt_t.
       */
      virtual pmt::pmt_t get_message(int i) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_DEBUG_H */
