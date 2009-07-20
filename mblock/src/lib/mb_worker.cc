/* -*- c++ -*- */
/*
 * Copyright 2007,2008,2009 Free Software Foundation, Inc.
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
#include <mb_worker.h>
#include <mb_runtime_thread_per_block.h>
#include <mblock/exception.h>
#include <mblock/mblock.h>
#include <mb_gettid.h>
#include <mblock/msg_accepter.h>
#include <iostream>
#include <cstdio>
#ifdef HAVE_SCHED_H
#include <sched.h>
#endif

#define VERBOSE 0		// define to 0 or 1


static gruel::pmt_t s_worker_state_changed = gruel::pmt_intern("%worker-state-changed");


mb_worker::mb_worker(mb_runtime_thread_per_block *runtime,
		     mb_mblock_maker_t maker,
		     const std::string &instance_name,
		     gruel::pmt_t user_arg)
  : omni_thread((void *) 0, PRIORITY_NORMAL),
    d_runtime(runtime), d_maker(maker),
    d_instance_name(instance_name), d_user_arg(user_arg),
    d_state_cond(&d_mutex), d_state(TS_UNINITIALIZED),
    d_why_dead(RIP_NOT_DEAD_YET)
{
}

#if 0
mb_worker::~mb_worker()
{
}
#endif

#ifdef HAVE_SCHED_SETAFFINITY
static void
set_affinity(const std::string &instance_name, const std::string &class_name)
{
  //static int	counter = 0;
  cpu_set_t	mask;
  CPU_ZERO(&mask);

  if (0){

    //CPU_SET(counter & 0x1, &mask);
    //counter++;
    CPU_SET(0, &mask);

    int r = sched_setaffinity(mb_gettid(), sizeof(mask), &mask);
    if (r == -1)
      perror("sched_setaffinity");
  }
}
#else
static void
set_affinity(const std::string &instance_name, const std::string &class_name)
{
}
#endif

void
mb_worker::set_state(worker_state_t state)
{
  {
    omni_mutex_lock  l2(d_mutex);

    d_state = state;			  // update our state
    d_state_cond.broadcast();		  // Notify everybody who cares...
  }

  // send msg to runtime, telling it something changed.
  (*d_runtime->accepter())(s_worker_state_changed, gruel::PMT_F, gruel::PMT_F, MB_PRI_BEST);
}

void *
mb_worker::run_undetached(void *ignored)
{
  // FIXME add pthread_sigmask stuff

  //set_affinity(d_instance_name, d_class_name);
  set_affinity(d_instance_name, "");

  try {
    worker_thread_top_level();
    d_why_dead = RIP_EXIT;
  }
  catch (mbe_terminate){
    d_why_dead = RIP_TERMINATE;
  }
  catch (mbe_exit){
    d_why_dead = RIP_EXIT;
  }
  catch (std::logic_error e){
    if (d_why_dead == RIP_NOT_DEAD_YET)
      d_why_dead = RIP_UNHANDLED_EXCEPTION;

    std::cerr << "\nmb_worker::run_undetached: unhandled exception:\n";
    std::cerr << "  " << e.what() << std::endl;
  }
  catch (...){
    if (d_why_dead == RIP_NOT_DEAD_YET)
      d_why_dead = RIP_UNHANDLED_EXCEPTION;
  }

  if (VERBOSE)
    std::cerr << "\nrun_undetached: about to return, d_why_dead = "
	      << d_why_dead << std::endl;

  set_state(TS_DEAD);
  return 0;
}

void
mb_worker::worker_thread_top_level()
{
  if (VERBOSE)
    std::cerr << "worker_thread_top_level (enter):" << std::endl
	      << "  instance_name: " << d_instance_name << std::endl
	      << "  omnithread id: " << id() << std::endl
	      << "  gettid:        " << mb_gettid() << std::endl
	      << "  getpid:        " << getpid() << std::endl;

  cause_of_death_t pending_cause_of_death = RIP_NOT_DEAD_YET;
  
  try {
    pending_cause_of_death = RIP_CTOR_EXCEPTION;
    d_mblock = d_maker(d_runtime, d_instance_name, d_user_arg);

    if (VERBOSE)
      std::cerr << "worker_thread_top_level (post-construction):" << std::endl
		<< "  instance_name: " << d_instance_name << std::endl;

    pending_cause_of_death = RIP_INIT_EXCEPTION;
    d_mblock->initial_transition();

    if (VERBOSE)
      std::cerr << "worker_thread_top_level (post-initial-transition):" << std::endl
		<< "  instance_name: " << d_instance_name << std::endl;

    set_state(TS_RUNNING);

    pending_cause_of_death = RIP_UNHANDLED_EXCEPTION;
    d_mblock->main_loop();
  }
  catch (...){
    d_why_dead = pending_cause_of_death;
    throw;
  }

  if (VERBOSE)
    std::cerr << "worker_thread_top_level (exit):" << std::endl
	      << "  instance_name: " << d_instance_name << std::endl;
}
