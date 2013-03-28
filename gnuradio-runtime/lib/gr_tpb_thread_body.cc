/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2011 Free Software Foundation, Inc.
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
#include <gr_tpb_thread_body.h>
#include <gr_prefs.h>
#include <iostream>
#include <boost/thread.hpp>
#include <gruel/pmt.h>
#include <boost/foreach.hpp>

using namespace pmt;

gr_tpb_thread_body::gr_tpb_thread_body(gr_block_sptr block, int max_noutput_items)
  : d_exec(block, max_noutput_items)
{
  //std::cerr << "gr_tpb_thread_body: " << block << std::endl;

  gr_block_detail *d = block->detail().get();
  gr_block_executor::state s;
  pmt_t msg;

  d->threaded = true;
  d->thread = gruel::get_current_thread_id();

  gr_prefs *p = gr_prefs::singleton();
  size_t max_nmsgs = static_cast<size_t>(p->get_long("DEFAULT", "max_messages", 100));

  // Set thread affinity if it was set before fg was started.
  if(block->processor_affinity().size() > 0) {
    gruel::thread_bind_to_processor(d->thread, block->processor_affinity());
  }

  while (1){
    boost::this_thread::interruption_point();

    // handle any queued up messages
    //BOOST_FOREACH( pmt::pmt_t port, block->msg_queue.keys() )
    
    BOOST_FOREACH( gr_basic_block::msg_queue_map_t::value_type &i, block->msg_queue )
    {
      // Check if we have a message handler attached before getting
      // any messages. This is mostly a protection for the unknown
      // startup sequence of the threads.
      if(block->has_msg_handler(i.first)) {
        while ((msg = block->delete_head_nowait(i.first))){
          block->dispatch_msg(i.first,msg);
        }
      }
      else {
        // If we don't have a handler but are building up messages,
        // prune the queue from the front to keep memory in check.
        if(block->nmsgs(i.first) > max_nmsgs)
          msg = block->delete_head_nowait(i.first);
      }
    }

    d->d_tpb.clear_changed();
    // run one iteration if we are a connected stream block
    if(d->noutputs() >0 || d->ninputs()>0){
        s = d_exec.run_one_iteration();
    } else {
        s = gr_block_executor::BLKD_IN;
    }

    switch(s){
    case gr_block_executor::READY:		// Tell neighbors we made progress.
      d->d_tpb.notify_neighbors(d);
      break;

    case gr_block_executor::READY_NO_OUTPUT:	// Notify upstream only
      d->d_tpb.notify_upstream(d);
      break;

    case gr_block_executor::DONE:		// Game over.
      d->d_tpb.notify_neighbors(d);
      return;

    case gr_block_executor::BLKD_IN:		// Wait for input.
      {
	gruel::scoped_lock guard(d->d_tpb.mutex);
	while (!d->d_tpb.input_changed){

	  // wait for input or message
	  while(!d->d_tpb.input_changed && block->empty_p())
	    d->d_tpb.input_cond.wait(guard);

	  // handle all pending messages
      BOOST_FOREACH( gr_basic_block::msg_queue_map_t::value_type &i, block->msg_queue )
      {
	    while ((msg = block->delete_head_nowait(i.first))){
	      guard.unlock();			// release lock while processing msg
	      block->dispatch_msg(i.first, msg);
	      guard.lock();
	    }
      }
	}
      }
      break;


    case gr_block_executor::BLKD_OUT:		// Wait for output buffer space.
      {
	gruel::scoped_lock guard(d->d_tpb.mutex);
	while (!d->d_tpb.output_changed){

	  // wait for output room or message
	  while(!d->d_tpb.output_changed && block->empty_p())
	    d->d_tpb.output_cond.wait(guard);

	  // handle all pending messages
      BOOST_FOREACH( gr_basic_block::msg_queue_map_t::value_type &i, block->msg_queue )
      {
	    while ((msg = block->delete_head_nowait(i.first))){
	      guard.unlock();			// release lock while processing msg
	      block->dispatch_msg(i.first,msg);
	      guard.lock();
	    }
      }
	}
      }
      break;

    default:
      assert(0);
    }
  }
}

gr_tpb_thread_body::~gr_tpb_thread_body()
{
}
