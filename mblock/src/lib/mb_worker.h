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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INCLUDED_MB_WORKER_H
#define INCLUDED_MB_WORKER_H

#include <gnuradio/omnithread.h>
#include <mblock/common.h>
#include <mblock/class_registry.h>


class mb_worker;
//typedef boost::shared_ptr<mb_worker> mb_worker_sptr;

class mb_runtime_thread_per_block;

/*!
 * \brief Worker thread for thread_per_block runtime
 * \implementation
 */
class mb_worker : public omni_thread
{
public:
  //! worker thread states
  enum worker_state_t {
    TS_UNINITIALIZED,	// new, uninitialized
    TS_RUNNING,		// normal steady-state condition.
    TS_DEAD		// thread is dead
  };

  //! why we're dead
  enum cause_of_death_t {
    RIP_NOT_DEAD_YET,		// not dead
    RIP_EXIT,			// normal exit
    RIP_TERMINATE,		// caught terminate exception
    RIP_CTOR_EXCEPTION,		// constructor raised an exception
    RIP_INIT_EXCEPTION,		// initial_transition rasised an exception
    RIP_UNHANDLED_EXCEPTION	// somebody (most likely handle_message) raised an exception
  };

  /*
   * Args used by new thread to create mb_mblock
   */
  mb_runtime_thread_per_block  *d_runtime;
  mb_mblock_maker_t		d_maker;
  std::string			d_instance_name;
  pmt_t				d_user_arg;

  mb_mblock_sptr		d_mblock;	//< holds pointer to created mblock

  /*!
   * \brief General mutex for all these fields.
   *
   * They are accessed by both the main runtime thread and the newly
   * created thread that runs the mblock's main loop.
   */
  omni_mutex			d_mutex; 
  omni_condition		d_state_cond;	//< state change notifications
  worker_state_t		d_state;
  cause_of_death_t		d_why_dead;

  mb_worker(mb_runtime_thread_per_block *runtime,
	    mb_mblock_maker_t maker,
	    const std::string &instance_name,
	    pmt_t user_arg);

  // ~mb_worker();


  /*!
   * \brief This code runs as the top-level of the new thread
   */
  void worker_thread_top_level();
  
  /*!
   * \brief Invokes the top-level of the new thread (name kind of sucks)
   */
  void *run_undetached(void *arg);

private:
  // Neither d_mutex nor runtime->d_mutex may be held while calling this.
  // It locks and unlocks them itself.
  void set_state(worker_state_t state);
};



#endif /* INCLUDED_MB_WORKER_H */
