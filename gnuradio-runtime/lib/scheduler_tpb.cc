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

#include "scheduler_tpb.h"
#include "tpb_thread_body.h"
#include <gnuradio/thread/thread_body_wrapper.h>
#include <boost/make_shared.hpp>
#include <sstream>

namespace gr {

  class tpb_container
  {
    block_sptr d_block;
    int d_max_noutput_items;
    thread::barrier_sptr d_start_sync;

  public:
    tpb_container(block_sptr block, int max_noutput_items, thread::barrier_sptr start_sync)
      : d_block(block), d_max_noutput_items(max_noutput_items), d_start_sync(start_sync) {}

    void operator()()
    {
      tpb_thread_body body(d_block, d_start_sync, d_max_noutput_items);
    }
  };

  scheduler_sptr
  scheduler_tpb::make(flat_flowgraph_sptr ffg, int max_noutput_items)
  {
    return scheduler_sptr(new scheduler_tpb(ffg, max_noutput_items));
  }

  scheduler_tpb::scheduler_tpb(flat_flowgraph_sptr ffg,
                               int max_noutput_items)
    : scheduler(ffg, max_noutput_items)
  {
    int block_max_noutput_items;

    // Get a topologically sorted vector of all the blocks in use.
    // Being topologically sorted probably isn't going to matter, but
    // there's a non-zero chance it might help...

    basic_block_vector_t used_blocks = ffg->calc_used_blocks();
    used_blocks = ffg->topological_sort(used_blocks);
    block_vector_t blocks = flat_flowgraph::make_block_vector(used_blocks);

    // Ensure that the done flag is clear on all blocks

    for(size_t i = 0; i < blocks.size(); i++) {
      blocks[i]->detail()->set_done(false);
    }

    thread::barrier_sptr start_sync =
      boost::make_shared<thread::barrier>(blocks.size()+1);

    // Fire off a thead for each block

    for(size_t i = 0; i < blocks.size(); i++) {
      std::stringstream name;
      name << "thread-per-block[" << i << "]: " << blocks[i];

      // If set, use internal value instead of global value
      if(blocks[i]->is_set_max_noutput_items()) {
        block_max_noutput_items = blocks[i]->max_noutput_items();
      }
      else {
        block_max_noutput_items = max_noutput_items;
      }
      d_threads.create_thread(thread::thread_body_wrapper<tpb_container>(
          tpb_container(blocks[i], block_max_noutput_items, start_sync),
          name.str()));
    }
    start_sync->wait();
  }

  scheduler_tpb::~scheduler_tpb()
  {
    stop();
  }

  void
  scheduler_tpb::stop()
  {
    d_threads.interrupt_all();
  }

  void
  scheduler_tpb::wait()
  {
    d_threads.join_all();
  }

} /* namespace gr */
