/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "scheduler_sts.h"
#include "single_threaded_scheduler.h"
#include <gnuradio/thread/thread_body_wrapper.h>

namespace gr {

  class sts_container
  {
    block_vector_t d_blocks;

  public:
    sts_container(block_vector_t blocks)
      : d_blocks(blocks) {}

    void operator()()
    {
      make_single_threaded_scheduler(d_blocks)->run();
    }
  };

  scheduler_sptr
  scheduler_sts::make(flat_flowgraph_sptr ffg, int max_noutput_items)
  {
    return scheduler_sptr(new scheduler_sts(ffg, max_noutput_items));
  }

  scheduler_sts::scheduler_sts(flat_flowgraph_sptr ffg, int max_noutput_items)
    : scheduler(ffg, max_noutput_items)
  {
    // Split the flattened flow graph into discrete partitions, each
    // of which is topologically sorted.

    std::vector<basic_block_vector_t> graphs = ffg->partition();

    // For each partition, create a thread to evaluate it using
    // an instance of the gr_single_threaded_scheduler

    for(std::vector<basic_block_vector_t>::iterator p = graphs.begin();
        p != graphs.end(); p++) {

      block_vector_t blocks = flat_flowgraph::make_block_vector(*p);
      d_threads.create_thread(
        gr::thread::thread_body_wrapper<sts_container>(sts_container(blocks),
						  "single-threaded-scheduler"));
    }
  }

  scheduler_sts::~scheduler_sts()
  {
    stop();
  }

  void
  scheduler_sts::stop()
  {
    d_threads.interrupt_all();
  }

  void
  scheduler_sts::wait()
  {
    d_threads.join_all();
  }

} /* namespace gr */
