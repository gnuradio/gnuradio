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

#ifndef INCLUDED_GR_MESSAGE_STROBE_IMPL_H
#define INCLUDED_GR_MESSAGE_STROBE_IMPL_H

#include <gnuradio/blocks/message_strobe.h>

namespace gr {
  namespace blocks {

    class BLOCKS_API message_strobe_impl : public message_strobe
    {
    private:
      boost::shared_ptr<gr::thread::thread> d_thread;
      bool d_finished;
      float d_period_ms;
      pmt::pmt_t d_msg;
      const pmt::pmt_t d_port;

      void run();

    public:
      message_strobe_impl(pmt::pmt_t msg, float period_ms);
      ~message_strobe_impl();

      void set_msg(pmt::pmt_t msg) { d_msg = msg; }
      pmt::pmt_t msg() const { return d_msg; }
      void set_period(float period_ms) { d_period_ms = period_ms; }
      float period() const { return d_period_ms; }

      // Overloading these to start and stop the internal thread that
      // periodically produces the message.
      bool start();
      bool stop();
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_STROBE_IMPL_H */
