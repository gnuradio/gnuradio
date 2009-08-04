/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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
#include <iostream>
#include <boost/thread.hpp>
#include <gruel/pmt.h>

using namespace pmt;

gr_tpb_thread_body::gr_tpb_thread_body(gr_block_sptr block)
  : d_exec(block)
{
  // std::cerr << "gr_tpb_thread_body: " << block << std::endl;

  gr_block_detail *d = block->detail().get();
  gr_block_executor::state s;

  while (1){
    boost::this_thread::interruption_point();
 
    while (!block->msg_queue()->empty_p())
      block->handle_msg(block->msg_queue()->delete_head_nowait());

    d->d_tpb.clear_changed();
    s = d_exec.run_one_iteration();

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
      while (!d->d_tpb.input_changed) 
      {
	boost::this_thread::interruption_point();
	gruel::scoped_lock guard(d->d_tpb.mutex);
	
	// Block then wake on input_changed or msg arrived
	while(!d->d_tpb.input_changed && !block->msg_queue()->empty_p())
	  d->d_tpb.input_cond.wait(guard); 

	// Run msgq while unlocked
	guard.unlock();
	while (!block->msg_queue()->empty_p())
	  block->handle_msg(block->msg_queue()->delete_head_nowait());
      }
      break;

      
    case gr_block_executor::BLKD_OUT:		// Wait for output buffer space.
      while (!d->d_tpb.output_changed) 
      {
	boost::this_thread::interruption_point();
	gruel::scoped_lock guard(d->d_tpb.mutex);

	// Block then wake on output_changed or msg arrived
	while(!d->d_tpb.output_changed && !block->msg_queue()->empty_p())
	  d->d_tpb.output_cond.wait(guard); 

	// Run msgq while unlocked
	guard.unlock();
	while (!block->msg_queue()->empty_p())
	  block->handle_msg(block->msg_queue()->delete_head_nowait());
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
