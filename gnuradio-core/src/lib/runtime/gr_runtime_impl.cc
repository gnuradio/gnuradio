/* -*- c++ -*- */
/*
 * Copyright 2006,2007 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

#include <gr_runtime.h>
#include <gr_runtime_impl.h>
#include <gr_simple_flowgraph.h>
#include <gr_hier_block2.h>
#include <gr_hier_block2_detail.h>
#include <gr_local_sighandler.h>

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include <stdexcept>
#include <iostream>

#define GR_RUNTIME_IMPL_DEBUG 0

static gr_runtime_impl *s_runtime = 0;

// FIXME: This prevents using more than one gr_runtime instance
void 
runtime_sigint_handler(int signum)
{
  if (GR_RUNTIME_IMPL_DEBUG)
    std::cout << "SIGINT received, calling stop() on all threads" << std::endl;

  if (s_runtime)
    s_runtime->stop();
}

gr_runtime_impl::gr_runtime_impl(gr_hier_block2_sptr top_block, gr_runtime *owner) 
  : d_running(false),
    d_top_block(top_block),
    d_sfg(gr_make_simple_flowgraph()),
    d_owner(owner)
{
  s_runtime = this;
  top_block->set_runtime(d_owner);
}

gr_runtime_impl::~gr_runtime_impl()
{
  s_runtime = 0; // don't call delete we don't own these
  d_owner = 0;
}

void
gr_runtime_impl::start()
{
  if (GR_RUNTIME_IMPL_DEBUG)
    std::cout << "start: entered" << std::endl;

  if (d_running)
    throw std::runtime_error("already running");

  // Create new simple flow graph by flattening hierarchical block
  d_sfg->d_detail->reset();
  d_top_block->d_detail->flatten(d_sfg);

  // Validate new simple flow graph and wire it up
  d_sfg->d_detail->validate();
  d_sfg->d_detail->setup_connections();

  // Execute scheduler threads
  start_threads();
}

void
gr_runtime_impl::start_threads()
{
  if (GR_RUNTIME_IMPL_DEBUG)
    std::cout << "start_threads: entered" << std::endl;

  d_graphs = d_sfg->d_detail->partition();
  for (std::vector<gr_block_vector_t>::iterator p = d_graphs.begin();
       p != d_graphs.end(); p++) {
    gr_scheduler_thread *thread = new gr_scheduler_thread(*p);
    d_threads.push_back(thread);
    if (GR_RUNTIME_IMPL_DEBUG)
      std::cout << "start_threads: starting " << thread << std::endl;
    thread->start();
  }

  d_running = true;
}

void
gr_runtime_impl::stop()
{
  if (GR_RUNTIME_IMPL_DEBUG)
    std::cout << "stop: entered" << std::endl;

  for (gr_scheduler_thread_viter_t p = d_threads.begin(); p != d_threads.end(); p++) {
    if (GR_RUNTIME_IMPL_DEBUG)
      std::cout << "stop: stopping thread " << (*p) << std::endl;
    (*p)->stop();
  }

  d_running = false;
}

void
gr_runtime_impl::wait()
{
  if (GR_RUNTIME_IMPL_DEBUG)
    std::cout << "wait: entered" << std::endl;

  void *dummy_status; // don't ever dereference this
  gr_local_sighandler sigint(SIGINT, runtime_sigint_handler);

  for (gr_scheduler_thread_viter_t p = d_threads.begin(); p != d_threads.end(); p++) {
    if (GR_RUNTIME_IMPL_DEBUG)
      std::cout << "wait: joining thread " << (*p) << std::endl;
    (*p)->join(&dummy_status); // pthreads will self-delete, so pointer is now dead
    (*p) = 0; // FIXME: switch to stl::list and actually remove from container
    if (GR_RUNTIME_IMPL_DEBUG)
      std::cout << "wait: join returned" << std::endl;
  }

  d_threads.clear();
}


// N.B. lock() and unlock() cannot be called from a flow graph thread or
// deadlock will occur when reconfiguration happens
void
gr_runtime_impl::lock()
{
  omni_mutex_lock lock(d_reconf);
  d_lock_count++;
  if (GR_RUNTIME_IMPL_DEBUG)
    std::cout << "runtime: locked, count = " << d_lock_count <<  std::endl;
}

void
gr_runtime_impl::unlock()
{
  omni_mutex_lock lock(d_reconf);
  if (d_lock_count == 0)
    throw std::runtime_error("unpaired unlock() call");

  d_lock_count--;
  if (GR_RUNTIME_IMPL_DEBUG)
    std::cout << "runtime: unlocked, count = " << d_lock_count << std::endl;

  if (d_lock_count == 0)
    restart();
}

void
gr_runtime_impl::restart()
{
  if (GR_RUNTIME_IMPL_DEBUG)
    std::cout << "restart: entered" << std::endl;

  if (!d_running)
    throw std::runtime_error("not running");

  // Stop scheduler threads and wait for completion
  stop();
  wait();
  if (GR_RUNTIME_IMPL_DEBUG)
    std::cout << "restart: threads stopped" << std::endl;

  // Create new simple flow graph 
  gr_simple_flowgraph_sptr new_sfg = gr_make_simple_flowgraph();
  d_top_block->d_detail->flatten(new_sfg);
  new_sfg->validate();
  new_sfg->d_detail->merge_connections(d_sfg);

  if (GR_RUNTIME_IMPL_DEBUG)
    std::cout << "restart: replacing old flow graph with new" << std::endl;
  d_sfg = new_sfg;

  start_threads();
}

gr_scheduler_thread::gr_scheduler_thread(gr_block_vector_t graph) :
  omni_thread(NULL, PRIORITY_NORMAL),
  d_sts(gr_make_single_threaded_scheduler(graph))
{
}

gr_scheduler_thread::~gr_scheduler_thread()
{
}

void gr_scheduler_thread::start()
{
  start_undetached();
}

void *
gr_scheduler_thread::run_undetached(void *arg)
{
  // First code to run in new thread context

  // Mask off SIGINT in this thread to gaurantee mainline thread gets signal
#ifdef HAVE_SIGPROCMASK
  sigset_t old_set;
  sigset_t new_set;
  sigemptyset(&new_set);
  sigaddset(&new_set, SIGINT);
  sigprocmask(SIG_BLOCK, &new_set, &old_set);
#endif
  // Run the single-threaded scheduler
  d_sts->run();
  return 0;
}

void
gr_scheduler_thread::stop()
{
  d_sts->stop();
}
