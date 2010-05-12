/* -*- c++ -*- */
/*
 * Copyright 2007,2010 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GC_CLIENT_THREAD_INFO_H
#define INCLUDED_GC_CLIENT_THREAD_INFO_H

#include <boost/thread.hpp>
#include <boost/utility.hpp>

enum gc_ct_state {
  CT_NOT_WAITING,
  CT_WAIT_ALL,
  CT_WAIT_ANY,
};

/*
 * \brief per client-thread data used by gc_job_manager
 *
 * "Client threads" are any threads that invoke methods on
 * gc_job_manager.  We use pthread_set_specific to store a pointer to
 * one of these for each thread that comes our way.
 */
class gc_client_thread_info : boost::noncopyable {
public:
  gc_client_thread_info() :
    d_free(1), d_cond(), d_state(CT_NOT_WAITING),
    d_jobs_done(0), d_njobs_waiting_for(0),
    d_jobs_waiting_for(0){ }

  ~gc_client_thread_info() {
    d_free = 1;
    d_state = CT_NOT_WAITING;
    d_jobs_done = 0;
    d_njobs_waiting_for = 0;
    d_jobs_waiting_for = 0;
  }

  //! is this cti free? (1->free, 0->in use)
  uint32_t	  d_free;

  //! which client info are we?
  uint16_t	  d_client_id;

  //! hold this mutex to manipulate anything below here
  boost::mutex	  d_mutex;

  //! signaled by event handler to wake client thread up
  boost::condition_variable d_cond;

  //! Is this client waiting?
  gc_ct_state	  d_state;
  
  //! Jobs that have finished and not yet been waited for (bitvector)
  unsigned long	 *d_jobs_done;

  //! # of jobs we're waiting for
  unsigned int    d_njobs_waiting_for;

  //! Jobs that client thread is waiting for
  gc_job_desc	 **d_jobs_waiting_for;

};

#endif /* INCLUDED_GC_CLIENT_THREAD_INFO_H */
