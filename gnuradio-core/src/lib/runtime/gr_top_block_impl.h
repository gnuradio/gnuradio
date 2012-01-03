/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_TOP_BLOCK_IMPL_H
#define INCLUDED_GR_TOP_BLOCK_IMPL_H

#include <gr_core_api.h>
#include <gr_scheduler.h>
#include <gruel/thread.h>

/*!
 *\brief Abstract implementation details of gr_top_block
 * \ingroup internal
 *
 * The actual implementation of gr_top_block. Separate class allows
 * decoupling of changes from dependent classes.
 *
 */
class GR_CORE_API gr_top_block_impl
{
public:
  gr_top_block_impl(gr_top_block *owner);
  ~gr_top_block_impl();

  // Create and start scheduler threads
  void start(int max_noutput_items=100000);

  // Signal scheduler threads to stop
  void stop();

  // Wait for scheduler threads to exit
  void wait();

  // Lock the top block to allow reconfiguration
  void lock();

  // Unlock the top block at end of reconfiguration
  void unlock();

  // Dump the flowgraph to stdout
  void dump();

  // Get the number of max noutput_items in the flowgraph
  int max_noutput_items();
  
  // Set the maximum number of noutput_items in the flowgraph
  void set_max_noutput_items(int nmax);
  
protected:
    
  enum tb_state { IDLE, RUNNING };

  gr_top_block                  *d_owner;
  gr_flat_flowgraph_sptr         d_ffg;
  gr_scheduler_sptr		 d_scheduler;

  gruel::mutex                   d_mutex;	// protects d_state and d_lock_count
  tb_state			 d_state;
  int                            d_lock_count;
  int                            d_max_noutput_items;
  
private:
  void restart();
};

#endif /* INCLUDED_GR_TOP_BLOCK_IMPL_H */
