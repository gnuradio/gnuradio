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
#include <gr_top_block_impl_sts.h>
#include <gr_flat_flowgraph.h>
#include <gr_scheduler_thread.h>
#include <gr_local_sighandler.h>

#include <stdexcept>
#include <iostream>
#include <string.h>
#include <unistd.h>

#define GR_TOP_BLOCK_IMPL_STS_DEBUG 0

static gr_top_block_impl *s_impl = 0;


// FIXME: This prevents using more than one gr_top_block instance

static void 
runtime_sigint_handler(int signum)
{
  if (GR_TOP_BLOCK_IMPL_STS_DEBUG){
    char *msg = "SIGINT received, calling stop()\n";
    ::write(1, msg, strlen(msg));	// write is OK to call from signal handler
  }

  if (s_impl)
    s_impl->stop();
}

// ----------------------------------------------------------------

gr_top_block_impl_sts::gr_top_block_impl_sts(gr_top_block *owner) 
  : gr_top_block_impl(owner)
{
  if (s_impl)
    throw std::logic_error("gr_top_block_impl_sts: multiple simultaneous gr_top_blocks not allowed");

  s_impl = this;
}

gr_top_block_impl_sts::~gr_top_block_impl_sts()
{
  s_impl = 0; // don't call delete we don't own these
}

void
gr_top_block_impl_sts::start_threads()
{
  if (GR_TOP_BLOCK_IMPL_STS_DEBUG)
    std::cout << "start_threads: entered" << std::endl;

  d_graphs = d_ffg->partition();
  for (std::vector<gr_basic_block_vector_t>::iterator p = d_graphs.begin();
       p != d_graphs.end(); p++) {
    gr_scheduler_thread *thread = new gr_scheduler_thread(make_gr_block_vector(*p));
    d_threads.push_back(thread);
    if (GR_TOP_BLOCK_IMPL_STS_DEBUG)
      std::cout << "start_threads: starting " << thread << std::endl;
    thread->start();
  }

  d_running = true;
}

/*
 * N.B. as currently implemented, it is possible that this may be
 * invoked by the SIGINT handler which is fragile as hell...
 */
void
gr_top_block_impl_sts::stop()
{
  if (GR_TOP_BLOCK_IMPL_STS_DEBUG){
    char *msg = "stop: entered\n";
    ::write(1, msg, strlen(msg));
  }

  for (gr_scheduler_thread_viter_t p = d_threads.begin(); p != d_threads.end(); p++) {
    if (*p)
      (*p)->stop();
  }
}

void
gr_top_block_impl_sts::wait()
{
  if (GR_TOP_BLOCK_IMPL_STS_DEBUG)
    std::cout << "wait: entered" << std::endl;

  void *dummy_status; // don't ever dereference this
  gr_local_sighandler sigint(SIGINT, runtime_sigint_handler);

  for (gr_scheduler_thread_viter_t p = d_threads.begin(); p != d_threads.end(); p++) {
    if (GR_TOP_BLOCK_IMPL_STS_DEBUG)
      std::cout << "wait: joining thread " << (*p) << std::endl;
    (*p)->join(&dummy_status); // omnithreads will self-delete, so pointer is now dead
    (*p) = 0; // FIXME: switch to stl::list and actually remove from container
    if (GR_TOP_BLOCK_IMPL_STS_DEBUG)
      std::cout << "wait: join returned" << std::endl;
  }

  d_threads.clear();
  d_running = false;
}
