/* -*- c++ -*- */
/*
 * Copyright 2007,2008,2013 Free Software Foundation, Inc.
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

#include "top_block_impl.h"
#include "flat_flowgraph.h"
#include "scheduler_sts.h"
#include "scheduler_tpb.h"
#include <gnuradio/top_block.h>
#include <gnuradio/prefs.h>

#include <stdexcept>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

namespace gr {

#define GR_TOP_BLOCK_IMPL_DEBUG 0

  typedef scheduler_sptr(*scheduler_maker)(flat_flowgraph_sptr ffg,
                                           int max_noutput_items);

  static struct scheduler_table {
    const char *name;
    scheduler_maker f;
  } scheduler_table[] = {
    { "TPB", scheduler_tpb::make },    // first entry is default
    { "STS", scheduler_sts::make }
  };

  static scheduler_sptr
  make_scheduler(flat_flowgraph_sptr ffg, int max_noutput_items)
  {
    static scheduler_maker factory = 0;

    if(factory == 0) {
      char *v = getenv("GR_SCHEDULER");
      if(!v)
        factory = scheduler_table[0].f;	// use default
      else {
        for(size_t i = 0; i < sizeof(scheduler_table)/sizeof(scheduler_table[0]); i++) {
          if(strcmp(v, scheduler_table[i].name) == 0) {
            factory = scheduler_table[i].f;
            break;
          }
        }
        if(factory == 0) {
          std::cerr << "warning: Invalid GR_SCHEDULER environment variable value \""
                    << v << "\".  Using \"" << scheduler_table[0].name << "\"\n";
          factory = scheduler_table[0].f;
        }
      }
    }
    return factory(ffg, max_noutput_items);
  }

  top_block_impl::top_block_impl(top_block *owner)
    : d_owner(owner), d_ffg(),
      d_state(IDLE), d_lock_count(0), d_retry_wait(false)
  {
  }

  top_block_impl::~top_block_impl()
  {
    if (d_lock_count) {
      std::cerr << "error: destroying locked block." << std::endl;
    }
    d_owner = 0;
  }

  void
  top_block_impl::start(int max_noutput_items)
  {
    gr::thread::scoped_lock l(d_mutex);

    d_max_noutput_items = max_noutput_items;

    if(d_state != IDLE)
      throw std::runtime_error("top_block::start: top block already running or wait() not called after previous stop()");

    if(d_lock_count > 0)
      throw std::runtime_error("top_block::start: can't start with flow graph locked");

    // Create new flat flow graph by flattening hierarchy
    d_ffg = d_owner->flatten();

    // Validate new simple flow graph and wire it up
    d_ffg->validate();
    d_ffg->setup_connections();

    // Only export perf. counters if ControlPort config param is
    // enabled and if the PerfCounter option 'export' is turned on.
    prefs *p = prefs::singleton();
    if(p->get_bool("ControlPort", "on", false) && p->get_bool("PerfCounters", "export", false))
      d_ffg->enable_pc_rpc();

    d_scheduler = make_scheduler(d_ffg, d_max_noutput_items);
    d_state = RUNNING;
  }

  void
  top_block_impl::stop()
  {
    gr::thread::scoped_lock lock(d_mutex);

    if(d_scheduler)
      d_scheduler->stop();

    d_state = IDLE;
  }

  void
  top_block_impl::wait()
  {
    do {
      wait_for_jobs();
      {
        gr::thread::scoped_lock lock(d_mutex);
        if (!d_lock_count) {
          if(d_retry_wait) {
            d_retry_wait = false;
            continue;
          }
          d_state = IDLE;
          break;
        }
        d_lock_cond.wait(lock);
      }
    } while(true);
  }

  void
  top_block_impl::wait_for_jobs()
  {
    if(d_scheduler)
      d_scheduler->wait();
  }

  // N.B. lock() and unlock() cannot be called from a flow graph
  // thread or deadlock will occur when reconfiguration happens
  void
  top_block_impl::lock()
  {
    gr::thread::scoped_lock lock(d_mutex);
    if(d_scheduler)
      d_scheduler->stop();
    d_lock_count++;
  }

  void
  top_block_impl::unlock()
  {
    gr::thread::scoped_lock lock(d_mutex);

    if(d_lock_count <= 0) {
      d_lock_count = 0;		// fix it, then complain
      throw std::runtime_error("unpaired unlock() call");
    }

    d_lock_count--;
    if(d_lock_count > 0 || d_state == IDLE) // nothing to do
      return;

    restart();
    d_lock_cond.notify_all();
  }

  /*
   * restart is called with d_mutex held
   */
  void
  top_block_impl::restart()
  {
    wait_for_jobs();

    // Create new simple flow graph
    flat_flowgraph_sptr new_ffg = d_owner->flatten();
    new_ffg->validate();		 // check consistency, sanity, etc
    new_ffg->merge_connections(d_ffg);   // reuse buffers, etc
    d_ffg = new_ffg;

    // Create a new scheduler to execute it
    d_scheduler = make_scheduler(d_ffg, d_max_noutput_items);
    d_retry_wait = true;
  }

  std::string
  top_block_impl::edge_list()
  {
    if(d_ffg)
      return d_ffg->edge_list();
    else
      return "";
  }

  std::string
  top_block_impl::msg_edge_list()
  {
    if(d_ffg)
      return d_ffg->msg_edge_list();
    else
      return "";
  }

  void
  top_block_impl::dump()
  {
    if(d_ffg)
      d_ffg->dump();
  }

  int
  top_block_impl::max_noutput_items()
  {
    return d_max_noutput_items;
  }

  void
  top_block_impl::set_max_noutput_items(int nmax)
  {
    d_max_noutput_items = nmax;
  }

} /* namespace gr */
