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

#ifndef INCLUDED_GR_SCHEDULER_THREAD_H
#define INCLUDED_GR_SCHEDULER_THREAD_H

#include <omnithread.h>
#include <gr_single_threaded_scheduler.h>
#include <gr_block.h>

// omnithread calls delete on itself after thread exits, so can't use shared ptr
class gr_scheduler_thread;
typedef std::vector<gr_scheduler_thread *> gr_scheduler_thread_vector_t;
typedef gr_scheduler_thread_vector_t::iterator gr_scheduler_thread_viter_t;

/*!
 *\brief A single thread of execution for the scheduler
 *
 * \ingroup internal
 * This class implements a single thread that runs undetached, and
 * invokes the single-threaded block scheduler.  The runtime makes
 * one of these for each distinct partition of a flowgraph and runs
 * them in parallel.
 *
 */
class gr_scheduler_thread : public omni_thread
{
private:
  gr_single_threaded_scheduler_sptr d_sts;    

public:
  gr_scheduler_thread(gr_block_vector_t graph);
  ~gr_scheduler_thread();

  virtual void *run_undetached(void *arg);
  void start();
  void stop();
};

#endif /* INCLUDED_GR_SCHEDULER_THREAD_H */
