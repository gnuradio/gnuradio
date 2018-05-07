/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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
#include <gnuradio/blocks/nop.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/blocks/null_sink.h>
#include <boost/test/unit_test.hpp>
#include <iostream>

#define VERBOSE 0

BOOST_AUTO_TEST_CASE(t0)
{
  if (VERBOSE) std::cout << "qa_top_block::t0()\n";

  gr::top_block_sptr tb = gr::make_top_block("top");

  BOOST_REQUIRE(tb);
}

BOOST_AUTO_TEST_CASE(t1_run)
{
  if (VERBOSE) std::cout << "qa_top_block::t1()\n";

  gr::top_block_sptr tb = gr::make_top_block("top");

  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr head = gr::blocks::head::make(sizeof(int), 100000);
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, dst, 0);
  tb->run();
}

BOOST_AUTO_TEST_CASE(t2_start_stop_wait)
{
  if(VERBOSE)
    std::cout << "qa_top_block::t2()\n";

  gr::top_block_sptr tb = gr::make_top_block("top");

  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr head = gr::blocks::head::make(sizeof(int), 100000);
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, dst, 0);

  tb->start();
  tb->stop();
  tb->wait();
}

BOOST_AUTO_TEST_CASE(t3_lock_unlock)
{
  if(VERBOSE)
    std::cout << "qa_top_block::t3()\n";

  gr::top_block_sptr tb = gr::make_top_block("top");

  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  tb->connect(src, 0, dst, 0);

  tb->start();

  tb->lock();
  tb->unlock();

  tb->stop();
  tb->wait();
}

BOOST_AUTO_TEST_CASE(t4_reconfigure)
{
  if(VERBOSE)
    std::cout << "qa_top_block::t4()\n";

  gr::top_block_sptr tb = gr::make_top_block("top");

  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr head = gr::blocks::head::make(sizeof(int), 100000);
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  // Start infinite flowgraph
  tb->connect(src, 0, dst, 0);
  tb->start();

  // Reconfigure with gr_head in the middle
  tb->lock();
  tb->disconnect(src, 0, dst, 0);
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, dst, 0);
  tb->unlock();

  // Wait for flowgraph to end on its own
  tb->wait();
}


BOOST_AUTO_TEST_CASE(t5_max_noutputs)
{
  if(VERBOSE)
    std::cout << "qa_top_block::t5()\n";

  gr::top_block_sptr tb = gr::make_top_block("top");

  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr head = gr::blocks::head::make(sizeof(int), 100000);
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  // Start infinite flowgraph
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, dst, 0);
  tb->start(100);
  tb->wait();
}

BOOST_AUTO_TEST_CASE(t6_reconfig_max_noutputs)
{
  if(VERBOSE)
    std::cout << "qa_top_block::t6()\n";

  gr::top_block_sptr tb = gr::make_top_block("top");

  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr head = gr::blocks::head::make(sizeof(int), 100000);
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  // Start infinite flowgraph
  tb->connect(src, 0, dst, 0);
  tb->start(100);

  // Reconfigure with gr_head in the middle
  tb->lock();
  tb->disconnect(src, 0, dst, 0);
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, dst, 0);
  tb->set_max_noutput_items(1000);
  head->set_max_noutput_items(500);
  tb->unlock();

  // Wait for flowgraph to end on its own
  tb->wait();
}

BOOST_AUTO_TEST_CASE(t7_max_noutputs_per_block)
{
  if(VERBOSE)
    std::cout << "qa_top_block::t7()\n";

  gr::top_block_sptr tb = gr::make_top_block("top");

  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr head = gr::blocks::head::make(sizeof(int), 100000);
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  head->set_max_noutput_items(100);

  // Start infinite flowgraph
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, dst, 0);
  tb->start();
  tb->wait();
}

BOOST_AUTO_TEST_CASE(t8_reconfig_max_noutputs_per_block)
{
  if(VERBOSE)
    std::cout << "qa_top_block::t8()\n";

  gr::top_block_sptr tb = gr::make_top_block("top");

  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr head = gr::blocks::head::make(sizeof(int), 100000);
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  head->set_max_noutput_items(99);

  // Start infinite flowgraph
  tb->connect(src, 0, dst, 0);
  tb->start(201);

  // Reconfigure with gr_head in the middle
  tb->lock();
  tb->disconnect(src, 0, dst, 0);
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, dst, 0);
  tb->set_max_noutput_items(1023);
  head->set_max_noutput_items(513);
  tb->unlock();

  // Wait for flowgraph to end on its own
  tb->wait();
}

BOOST_AUTO_TEST_CASE(t9_max_output_buffer)
{
  if(VERBOSE)
    std::cout << "qa_top_block::t9()\n";

  gr::top_block_sptr tb = gr::make_top_block("top");

  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr head = gr::blocks::head::make(sizeof(int), 100000);
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  head->set_max_output_buffer(1024);

  // Start infinite flowgraph
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, dst, 0);
  tb->start();
  tb->wait();
}

BOOST_AUTO_TEST_CASE(t10_reconfig_max_output_buffer)
{
  if(VERBOSE)
    std::cout << "qa_top_block::t10()\n";

  gr::top_block_sptr tb = gr::make_top_block("top");

  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr head = gr::blocks::head::make(sizeof(int), 100000);
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  head->set_max_output_buffer(1000);

  // Start infinite flowgraph
  tb->connect(src, 0, dst, 0);
  tb->start(201);

  // Reconfigure with gr_head in the middle
  tb->lock();
  gr::block_sptr nop = gr::blocks::nop::make(sizeof(int));
  nop->set_max_output_buffer(4000);
  tb->disconnect(src, 0, dst, 0);
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, nop, 0);
  tb->connect(nop, 0, dst, 0);
  tb->unlock();

  // Wait for flowgraph to end on its own
  tb->wait();
}

BOOST_AUTO_TEST_CASE(t11_set_block_affinity)
{
  gr::top_block_sptr tb = gr::make_top_block("top");
  gr::block_sptr src (gr::blocks::null_source::make(sizeof(float)));
  gr::block_sptr snk (gr::blocks::null_sink::make(sizeof(float)));

  std::vector<int> set(1, 0), ret;
  src->set_processor_affinity(set);

  tb->connect(src, 0, snk, 0);
  tb->start();
  tb->stop();
  tb->wait();

  ret = src->processor_affinity();

  // We only set the core affinity to 0 because we always know at
  // least one thread core exists to use.
  BOOST_CHECK_EQUAL(set[0], ret[0]);
}
