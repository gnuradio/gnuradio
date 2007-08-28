/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#include <gr_scheduler_thread.h>
#include <iostream>

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#define GR_SCHEDULER_THREAD_DEBUG 0

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
  if (GR_SCHEDULER_THREAD_DEBUG)
    std::cout << "gr_scheduler_thread::start() "
	      << this << std::endl;
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
  if (GR_SCHEDULER_THREAD_DEBUG)
    std::cout << "gr_scheduler_thread::stop() "
	      << this << std::endl;
  d_sts->stop();
}
