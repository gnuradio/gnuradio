/* -*- c++ -*- */
/*
 * Copyright 2007,2011 Free Software Foundation, Inc.
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

#include <qa_set_msg_handler.h>
#include <gr_top_block.h>
#include <gr_head.h>
#include <gr_null_source.h>
#include <gr_null_sink.h>
#include <gr_nop.h>
#include <gruel/msg_passing.h>
#include <iostream>
#include <boost/thread/thread.hpp>


#define VERBOSE 0

using namespace pmt;

/*
 * The gr_nop block has been instrumented so that it counts
 * the number of messages sent to it.  We use this feature
 * to confirm that gr_nop's call to set_msg_handler is working
 * correctly.
 */

void qa_set_msg_handler::t0()
{
  static const int NMSGS = 10;

  if (VERBOSE) std::cout << "qa_set_msg_handler::t0()\n";

  gr_top_block_sptr tb = gr_make_top_block("top");

  gr_block_sptr src = gr_make_null_source(sizeof(int));
  gr_nop_sptr nop = gr_make_nop(sizeof(int));
  gr_block_sptr dst = gr_make_null_sink(sizeof(int));

  tb->connect(src, 0, nop, 0);
  tb->connect(nop, 0, dst, 0);

  // Must start graph before sending messages
  tb->start();

  // Send them...
  for (int i = 0; i < NMSGS; i++){
    send(nop, mp(mp("example-msg"), mp(i)));
  }

  // And send a message to null_source to confirm that the default
  // message handling action (which should be a nop) doesn't dump
  // core.
  send(src, mp(mp("example-msg"), mp(0)));

  // Give the messages a chance to be processed
  boost::this_thread::sleep(boost::posix_time::milliseconds(100));

  tb->stop();
  tb->wait();

  // Confirm that the nop block received the right number of messages.
  CPPUNIT_ASSERT_EQUAL(NMSGS, nop->nmsgs_received());
}
