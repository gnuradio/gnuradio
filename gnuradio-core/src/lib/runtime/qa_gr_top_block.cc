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

#include <qa_gr_top_block.h>
#include <gr_top_block.h>
#include <gr_head.h>
#include <gr_nop.h>
#include <gr_null_source.h>
#include <gr_null_sink.h>
#include <iostream>

#define VERBOSE 0

void qa_gr_top_block::t0()
{
  if (VERBOSE) std::cout << "qa_gr_top_block::t0()\n";

  gr_top_block_sptr tb = gr_make_top_block("top");

  CPPUNIT_ASSERT(tb);
}

void qa_gr_top_block::t1_run()
{
  if (VERBOSE) std::cout << "qa_gr_top_block::t1()\n";

  gr_top_block_sptr tb = gr_make_top_block("top");

  gr_block_sptr src = gr_make_null_source(sizeof(int));
  gr_block_sptr head = gr_make_head(sizeof(int), 100000);
  gr_block_sptr dst = gr_make_null_sink(sizeof(int));

  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, dst, 0);
  tb->run();
}

void qa_gr_top_block::t2_start_stop_wait()
{
  if (VERBOSE) std::cout << "qa_gr_top_block::t2()\n";

  gr_top_block_sptr tb = gr_make_top_block("top");

  gr_block_sptr src = gr_make_null_source(sizeof(int));
  gr_block_sptr head = gr_make_head(sizeof(int), 100000);
  gr_block_sptr dst = gr_make_null_sink(sizeof(int));

  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, dst, 0);

  tb->start();
  tb->stop();
  tb->wait();
}

void qa_gr_top_block::t3_lock_unlock()
{
  if (VERBOSE) std::cout << "qa_gr_top_block::t3()\n";

  gr_top_block_sptr tb = gr_make_top_block("top");

  gr_block_sptr src = gr_make_null_source(sizeof(int));
  gr_block_sptr dst = gr_make_null_sink(sizeof(int));

  tb->connect(src, 0, dst, 0);

  tb->start();

  tb->lock();
  tb->unlock();

  tb->stop();
  tb->wait();
}

void qa_gr_top_block::t4_reconfigure()
{
  if (VERBOSE) std::cout << "qa_gr_top_block::t4()\n";

  gr_top_block_sptr tb = gr_make_top_block("top");

  gr_block_sptr src = gr_make_null_source(sizeof(int));
  gr_block_sptr head = gr_make_head(sizeof(int), 100000);
  gr_block_sptr dst = gr_make_null_sink(sizeof(int));

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


void qa_gr_top_block::t5_max_noutputs()
{
  if (VERBOSE) std::cout << "qa_gr_top_block::t5()\n";

  gr_top_block_sptr tb = gr_make_top_block("top");

  gr_block_sptr src = gr_make_null_source(sizeof(int));
  gr_block_sptr head = gr_make_head(sizeof(int), 100000);
  gr_block_sptr dst = gr_make_null_sink(sizeof(int));

  // Start infinite flowgraph
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, dst, 0);
  tb->start(100);
  tb->wait();
}

void qa_gr_top_block::t6_reconfig_max_noutputs()
{
  if (VERBOSE) std::cout << "qa_gr_top_block::t6()\n";

  gr_top_block_sptr tb = gr_make_top_block("top");

  gr_block_sptr src = gr_make_null_source(sizeof(int));
  gr_block_sptr head = gr_make_head(sizeof(int), 100000);
  gr_block_sptr dst = gr_make_null_sink(sizeof(int));

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

void qa_gr_top_block::t7_max_noutputs_per_block()
{
  if (VERBOSE) std::cout << "qa_gr_top_block::t7()\n";

  gr_top_block_sptr tb = gr_make_top_block("top");

  gr_block_sptr src = gr_make_null_source(sizeof(int));
  gr_block_sptr head = gr_make_head(sizeof(int), 100000);
  gr_block_sptr dst = gr_make_null_sink(sizeof(int));

  head->set_max_noutput_items(100);

  // Start infinite flowgraph
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, dst, 0);
  tb->start();
  tb->wait();
}

void qa_gr_top_block::t8_reconfig_max_noutputs_per_block()
{
  if (VERBOSE) std::cout << "qa_gr_top_block::t8()\n";

  gr_top_block_sptr tb = gr_make_top_block("top");

  gr_block_sptr src = gr_make_null_source(sizeof(int));
  gr_block_sptr head = gr_make_head(sizeof(int), 100000);
  gr_block_sptr dst = gr_make_null_sink(sizeof(int));

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

void qa_gr_top_block::t9_max_output_buffer()
{
  if (VERBOSE) std::cout << "qa_gr_top_block::t9()\n";

  gr_top_block_sptr tb = gr_make_top_block("top");

  gr_block_sptr src = gr_make_null_source(sizeof(int));
  gr_block_sptr head = gr_make_head(sizeof(int), 100000);
  gr_block_sptr dst = gr_make_null_sink(sizeof(int));

  head->set_max_output_buffer(1024);

  // Start infinite flowgraph
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, dst, 0);
  tb->start();
  tb->wait();
}

void qa_gr_top_block::t10_reconfig_max_output_buffer()
{
  if (VERBOSE) std::cout << "qa_gr_top_block::t10()\n";

  gr_top_block_sptr tb = gr_make_top_block("top");

  gr_block_sptr src = gr_make_null_source(sizeof(int));
  gr_block_sptr head = gr_make_head(sizeof(int), 100000);
  gr_block_sptr dst = gr_make_null_sink(sizeof(int));

  head->set_max_output_buffer(1000);

  // Start infinite flowgraph
  tb->connect(src, 0, dst, 0);
  tb->start(201);

  // Reconfigure with gr_head in the middle
  tb->lock();
  gr_block_sptr nop = gr_make_nop(sizeof(int));
  nop->set_max_output_buffer(4000);
  tb->disconnect(src, 0, dst, 0);
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, nop, 0);
  tb->connect(nop, 0, dst, 0);
  tb->unlock();

  // Wait for flowgraph to end on its own
  tb->wait();
}
