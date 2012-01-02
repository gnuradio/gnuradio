/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
#include <gr_scheduler_sts.h>
#include <gr_single_threaded_scheduler.h>
#include <gruel/thread_body_wrapper.h>

class sts_container
{
  gr_block_vector_t	d_blocks;
  
public:

  sts_container(gr_block_vector_t blocks)
    : d_blocks(blocks) {}

  void operator()()
  {
    gr_make_single_threaded_scheduler(d_blocks)->run();
  }
};


gr_scheduler_sptr
gr_scheduler_sts::make(gr_flat_flowgraph_sptr ffg, int max_noutput_items)
{
  return gr_scheduler_sptr(new gr_scheduler_sts(ffg, max_noutput_items));
}

gr_scheduler_sts::gr_scheduler_sts(gr_flat_flowgraph_sptr ffg, int max_noutput_items)
  : gr_scheduler(ffg, max_noutput_items)
{
  // Split the flattened flow graph into discrete partitions, each
  // of which is topologically sorted.

  std::vector<gr_basic_block_vector_t> graphs = ffg->partition();

  // For each partition, create a thread to evaluate it using
  // an instance of the gr_single_threaded_scheduler

  for (std::vector<gr_basic_block_vector_t>::iterator p = graphs.begin();
       p != graphs.end(); p++) {

    gr_block_vector_t blocks = gr_flat_flowgraph::make_block_vector(*p);
    d_threads.create_thread(
        gruel::thread_body_wrapper<sts_container>(sts_container(blocks),
						  "single-threaded-scheduler"));
  }
}

gr_scheduler_sts::~gr_scheduler_sts()
{
  stop();
}

void
gr_scheduler_sts::stop()
{
  d_threads.interrupt_all();
}

void
gr_scheduler_sts::wait()
{
  d_threads.join_all();
}
