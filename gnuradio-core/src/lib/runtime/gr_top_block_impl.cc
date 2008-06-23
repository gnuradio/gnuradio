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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_top_block.h>
#include <gr_top_block_impl.h>
#include <gr_flat_flowgraph.h>
#include <gr_scheduler_thread.h>
#include <gr_local_sighandler.h>

#include <stdexcept>
#include <iostream>
#include <string.h>
#include <unistd.h>

#define GR_TOP_BLOCK_IMPL_DEBUG 0

gr_top_block_impl::gr_top_block_impl(gr_top_block *owner) 
  : d_owner(owner),
    d_running(false),
    d_ffg(),
    d_lock_count(0)
{
}

gr_top_block_impl::~gr_top_block_impl()
{
  d_owner = 0;
}

void
gr_top_block_impl::start()
{
  if (GR_TOP_BLOCK_IMPL_DEBUG)
    std::cout << "start: entered " << this << std::endl;

  if (d_running)
    throw std::runtime_error("top block already running or wait() not called after previous stop()");

  // Create new flat flow graph by flattening hierarchy
  d_ffg = d_owner->flatten();

  // Validate new simple flow graph and wire it up
  d_ffg->validate();
  d_ffg->setup_connections();

  // Execute scheduler threads
  start_threads();
}


// N.B. lock() and unlock() cannot be called from a flow graph thread or
// deadlock will occur when reconfiguration happens
void
gr_top_block_impl::lock()
{
  omni_mutex_lock lock(d_reconf);
  d_lock_count++;
  if (GR_TOP_BLOCK_IMPL_DEBUG)
    std::cout << "runtime: locked, count = " << d_lock_count <<  std::endl;
}

void
gr_top_block_impl::unlock()
{
  omni_mutex_lock lock(d_reconf);
  if (d_lock_count <= 0)
    throw std::runtime_error("unpaired unlock() call");

  d_lock_count--;
  if (GR_TOP_BLOCK_IMPL_DEBUG)
    std::cout << "unlock: unlocked, count = " << d_lock_count << std::endl;

  if (d_lock_count == 0) {
    if (GR_TOP_BLOCK_IMPL_DEBUG)
      std::cout << "unlock: restarting flowgraph" << std::endl;
    restart();
  }
}

void
gr_top_block_impl::restart()
{
  if (GR_TOP_BLOCK_IMPL_DEBUG)
    std::cout << "restart: entered" << std::endl;

  if (!d_running)
    throw std::runtime_error("top block is not running");

  // Stop scheduler threads and wait for completion
  stop();
  wait();
  if (GR_TOP_BLOCK_IMPL_DEBUG)
    std::cout << "restart: threads stopped" << std::endl;

  // Create new simple flow graph
  gr_flat_flowgraph_sptr new_ffg = d_owner->flatten();        
  new_ffg->validate();		       // check consistency, sanity, etc

  if (GR_TOP_BLOCK_IMPL_DEBUG) {
      std::cout << std::endl << "*** Existing flat flowgraph @" << d_ffg << ":" << std::endl;
      d_ffg->dump();
  }
  new_ffg->merge_connections(d_ffg);   // reuse buffers, etc

  if (GR_TOP_BLOCK_IMPL_DEBUG) {
    std::cout << std::endl << "*** New flat flowgraph after merge @" << new_ffg << ":" << std::endl;
    new_ffg->dump();
  }
  
  d_ffg = new_ffg;

  start_threads();
}

void
gr_top_block_impl::dump()
{
  if (d_ffg)
    d_ffg->dump();
}

gr_block_vector_t
gr_top_block_impl::make_gr_block_vector(gr_basic_block_vector_t blocks)
{
  gr_block_vector_t result;
  for (gr_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
    result.push_back(make_gr_block_sptr(*p));
  }

  return result;
}
