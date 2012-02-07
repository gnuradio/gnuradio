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
#include <gr_scheduler_tpb.h>
#include <gr_tpb_thread_body.h>
#include <gruel/thread_body_wrapper.h>
#include <sstream>

/*
 * You know, a lambda expression would be sooo much easier...
 */
class tpb_container
{
  gr_block_sptr	d_block;
  int d_max_noutput_items;
  
public:
  tpb_container(gr_block_sptr block, int max_noutput_items)
    : d_block(block), d_max_noutput_items(max_noutput_items) {}

  void operator()()
  {
    gr_tpb_thread_body	body(d_block, d_max_noutput_items);
  }
};


gr_scheduler_sptr
gr_scheduler_tpb::make(gr_flat_flowgraph_sptr ffg, int max_noutput_items)
{
  return gr_scheduler_sptr(new gr_scheduler_tpb(ffg, max_noutput_items));
}

gr_scheduler_tpb::gr_scheduler_tpb(gr_flat_flowgraph_sptr ffg, int max_noutput_items)
  : gr_scheduler(ffg, max_noutput_items)
{
  // Get a topologically sorted vector of all the blocks in use.
  // Being topologically sorted probably isn't going to matter, but
  // there's a non-zero chance it might help...

  gr_basic_block_vector_t used_blocks = ffg->calc_used_blocks();
  used_blocks = ffg->topological_sort(used_blocks);
  gr_block_vector_t blocks = gr_flat_flowgraph::make_block_vector(used_blocks);

  // Ensure that the done flag is clear on all blocks

  for (size_t i = 0; i < blocks.size(); i++){
    blocks[i]->detail()->set_done(false);
  }

  // Fire off a thead for each block

  for (size_t i = 0; i < blocks.size(); i++){
    std::stringstream name;
    name << "thread-per-block[" << i << "]: " << blocks[i];
    d_threads.create_thread(
	    gruel::thread_body_wrapper<tpb_container>(tpb_container(blocks[i], max_noutput_items),
						      name.str()));
  }
}

gr_scheduler_tpb::~gr_scheduler_tpb()
{
  stop();
}

void
gr_scheduler_tpb::stop()
{
  d_threads.interrupt_all();
}

void
gr_scheduler_tpb::wait()
{
  d_threads.join_all();
}
