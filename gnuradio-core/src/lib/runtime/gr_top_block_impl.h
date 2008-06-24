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

#include <gr_scheduler_thread.h>

/*!
 *\brief Abstract implementation details of gr_top_block
 * \ingroup internal
 *
 * The actual implementation of gr_top_block. Separate class allows
 * decoupling of changes from dependent classes.
 *
 */
class gr_top_block_impl
{
public:
  gr_top_block_impl(gr_top_block *owner);
  virtual ~gr_top_block_impl();

  // Create and start scheduler threads
  virtual void start();

  // Signal scheduler threads to stop
  virtual void stop() = 0;

  // Wait for scheduler threads to exit
  virtual void wait() = 0;

  // Lock the top block to allow reconfiguration
  void lock();

  // Unlock the top block at end of reconfiguration
  void unlock();

  // Dump the flowgraph to stdout
  void dump();
  
protected:
    
  gr_top_block                  *d_owner;
  bool                           d_running;
  gr_flat_flowgraph_sptr         d_ffg;

  omni_mutex                     d_reconf;	// protects d_lock_count
  int                            d_lock_count;

  virtual void start_threads() = 0;

/*!
 * Make a vector of gr_block from a vector of gr_basic_block
 *
 * Pass-by-value to avoid problem with possible asynchronous modification
 */
  static gr_block_vector_t make_gr_block_vector(gr_basic_block_vector_t blocks);

private:
  void restart();
};

#endif /* INCLUDED_GR_TOP_BLOCK_IMPL_H */
