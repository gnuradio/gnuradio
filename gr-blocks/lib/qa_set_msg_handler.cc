/* -*- c++ -*- */
/*
 * Copyright 2007,2011,2013 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <gnuradio/top_block.h>
#include <gnuradio/blocks/head.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/nop.h>
#include <gnuradio/messages/msg_passing.h>
#include <boost/thread/thread.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>

/*
 * The gr::block::nop block has been instrumented so that it counts
 * the number of messages sent to it. We use this feature to confirm
 * that gr::blocks::nop's call to set_msg_handler is working correctly.
 */
BOOST_AUTO_TEST_CASE(t0)
{
  static const int NMSGS = 10;

  gr::top_block_sptr tb = gr::make_top_block("top");
  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::blocks::nop::sptr nop = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  tb->connect(src, 0, nop, 0);
  tb->connect(nop, 0, dst, 0);

  // Must start graph before sending messages
  tb->start();

  // Send them...
  pmt::pmt_t port(pmt::intern("port"));
  for (int i = 0; i < NMSGS; i++){
    send(nop, port, pmt::mp(pmt::mp("example-msg"), pmt::mp(i)));
  }

  // Give the messages a chance to be processed
  boost::this_thread::sleep(boost::posix_time::milliseconds(100));

  tb->stop();
  tb->wait();

  // Confirm that the nop block received the right number of messages.
  BOOST_CHECK_EQUAL(NMSGS, nop->nmsgs_received());
}
