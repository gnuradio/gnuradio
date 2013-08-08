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

#ifndef INCLUDED_GR_MESSAGE_STROBE_H
#define INCLUDED_GR_MESSAGE_STROBE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Send message at defined interval
     * \ingroup message_tools_blk
     *
     * \details
     * Takes a PMT message and sends it out every \p period_ms
     * milliseconds. Useful for testing/debugging the message system.
     */
    class BLOCKS_API message_strobe : virtual public block
    {
    public:
      // gr::blocks::message_strobe::sptr
      typedef boost::shared_ptr<message_strobe> sptr;

      /*!
       * Make a message stobe block to send message \p msg every \p
       * period_ms milliseconds.
       *
       * \param msg The message to send as a PMT.
       * \param period_ms the time period in milliseconds in which to
       *                  send \p msg.
       */
      static sptr make(pmt::pmt_t msg, float period_ms);

      /*!
       * Reset the message being sent.
       * \param msg The message to send as a PMT.
       */
      virtual void set_msg(pmt::pmt_t msg) = 0;

      /*!
       * Get the value of the message being sent.
       */
      virtual pmt::pmt_t msg() const = 0;

      /*!
       * Reset the sending interval.
       * \param period_ms the time period in milliseconds.
       */
      virtual void set_period(float period_ms) = 0;

      /*!
       * Get the time interval of the strobe.
       */
      virtual float period() const = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_STROBE_H */
