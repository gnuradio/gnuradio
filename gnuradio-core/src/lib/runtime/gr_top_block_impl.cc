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
#include <gr_scheduler_sts.h>
#include <gr_scheduler_tpb.h>

#include <stdexcept>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define GR_TOP_BLOCK_IMPL_DEBUG 0


typedef gr_scheduler_sptr (*scheduler_maker)(gr_flat_flowgraph_sptr ffg,
					     int max_noutput_items);

static struct scheduler_table {
  const char 	       *name;
  scheduler_maker	f;
} scheduler_table[] = {
  { "TPB",	gr_scheduler_tpb::make },	// first entry is default
  { "STS",	gr_scheduler_sts::make }
};

static gr_scheduler_sptr
make_scheduler(gr_flat_flowgraph_sptr ffg, int max_noutput_items)
{
  static scheduler_maker  factory = 0;

  if (factory == 0){
    char *v = getenv("GR_SCHEDULER");
    if (!v)
      factory = scheduler_table[0].f;	// use default
    else {
      for (size_t i = 0; i < sizeof(scheduler_table)/sizeof(scheduler_table[0]); i++){
	if (strcmp(v, scheduler_table[i].name) == 0){
	  factory = scheduler_table[i].f;
	  break;
	}
      }
      if (factory == 0){
	std::cerr << "warning: Invalid GR_SCHEDULER environment variable value \""
		  << v << "\".  Using \"" << scheduler_table[0].name << "\"\n";
	factory = scheduler_table[0].f;
      }
    }
  }
  return factory(ffg, max_noutput_items);
}


gr_top_block_impl::gr_top_block_impl(gr_top_block *owner) 
  : d_owner(owner), d_ffg(),
    d_state(IDLE), d_lock_count(0)
{
}

gr_top_block_impl::~gr_top_block_impl()
{
  d_owner = 0;
}

void
gr_top_block_impl::start(int max_noutput_items)
{
  gruel::scoped_lock	l(d_mutex);

  d_max_noutput_items = max_noutput_items;

  if (d_state != IDLE)
    throw std::runtime_error("top_block::start: top block already running or wait() not called after previous stop()");

  if (d_lock_count > 0)
    throw std::runtime_error("top_block::start: can't start with flow graph locked");

  // Create new flat flow graph by flattening hierarchy
  d_ffg = d_owner->flatten();

  // Validate new simple flow graph and wire it up
  d_ffg->validate();
  d_ffg->setup_connections();

  d_scheduler = make_scheduler(d_ffg, d_max_noutput_items);
  d_state = RUNNING;
}

void 
gr_top_block_impl::stop()
{
  if (d_scheduler)
    d_scheduler->stop();
}


void
gr_top_block_impl::wait()
{
  if (d_scheduler)
    d_scheduler->wait();

  d_state = IDLE;
}

// N.B. lock() and unlock() cannot be called from a flow graph thread or
// deadlock will occur when reconfiguration happens
void
gr_top_block_impl::lock()
{
  gruel::scoped_lock lock(d_mutex);
  d_lock_count++;
}

void
gr_top_block_impl::unlock()
{
  gruel::scoped_lock lock(d_mutex);

  if (d_lock_count <= 0){
    d_lock_count = 0;		// fix it, then complain
    throw std::runtime_error("unpaired unlock() call");
  }

  d_lock_count--;
  if (d_lock_count > 0 || d_state == IDLE) // nothing to do
    return;

  restart();
}

/*
 * restart is called with d_mutex held
 */
void
gr_top_block_impl::restart()
{
  stop();		     // Stop scheduler and wait for completion
  wait();

  // Create new simple flow graph
  gr_flat_flowgraph_sptr new_ffg = d_owner->flatten();        
  new_ffg->validate();		       // check consistency, sanity, etc
  new_ffg->merge_connections(d_ffg);   // reuse buffers, etc
  d_ffg = new_ffg;

  // Create a new scheduler to execute it
  d_scheduler = make_scheduler(d_ffg, d_max_noutput_items);
  d_state = RUNNING;
}

void
gr_top_block_impl::dump()
{
  if (d_ffg)
    d_ffg->dump();
}

int
gr_top_block_impl::max_noutput_items()
{
  return d_max_noutput_items;
}
  
void
gr_top_block_impl::set_max_noutput_items(int nmax)
{
  d_max_noutput_items = nmax;
}
