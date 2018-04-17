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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "message_strobe_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <string.h>
#include <iostream>

namespace gr {
  namespace blocks {

    message_strobe::sptr
    message_strobe::make(pmt::pmt_t msg, float period_ms)
    {
      return gnuradio::get_initial_sptr
        (new message_strobe_impl(msg, period_ms));
    }

    message_strobe_impl::message_strobe_impl(pmt::pmt_t msg, float period_ms)
      : block("message_strobe",
              io_signature::make(0, 0, 0),
              io_signature::make(0, 0, 0)),
        d_finished(false),
        d_period_ms(period_ms),
        d_msg(msg),
        d_port(pmt::mp("strobe"))
    {
      message_port_register_out(d_port);

      message_port_register_in(pmt::mp("set_msg"));
      set_msg_handler(pmt::mp("set_msg"),
                      boost::bind(&message_strobe_impl::set_msg, this, _1));
    }

    message_strobe_impl::~message_strobe_impl()
    {
    }

    bool
    message_strobe_impl::start()
    {
      // NOTE: d_finished should be something explicitly thread safe. But since
      // nothing breaks on concurrent access, I'll just leave it as bool.
      d_finished = false;
      d_thread = boost::shared_ptr<gr::thread::thread>
        (new gr::thread::thread(boost::bind(&message_strobe_impl::run, this)));

      return block::start();
    }

    bool
    message_strobe_impl::stop()
    {
      // Shut down the thread
      d_finished = true;
      d_thread->interrupt();
      d_thread->join();

      return block::stop();
    }

    void message_strobe_impl::run()
    {
      while(!d_finished) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(static_cast<long>(d_period_ms)));
        if(d_finished) {
          return;
        }

        message_port_pub(d_port, d_msg);
      }
    }

  } /* namespace blocks */
} /* namespace gr */
