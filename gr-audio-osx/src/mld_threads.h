/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio.
 *
 * Primary Author: Michael Dickens, NCIP Lab, University of Notre Dame
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

#ifndef _INCLUDED_MLD_THREADS_H_
#define _INCLUDED_MLD_THREADS_H_

/* classes which allow for either pthreads or omni_threads */

#define __macos__
#ifdef _USE_OMNI_THREADS_
#include <omnithread.h>
#else
#include <pthread.h>
#endif

#include <stdexcept>

#define __INLINE__ inline
#define DO_DEBUG 0

#if DO_DEBUG
#define DEBUG(X) do{X} while(0);
#else
#define DEBUG(X) do{} while(0);
#endif

class mld_condition_t;

class mld_mutex_t {
#ifdef _USE_OMNI_THREADS_
  typedef omni_mutex l_mutex, *l_mutex_ptr;
#else
  typedef pthread_mutex_t l_mutex, *l_mutex_ptr;
#endif

  friend class mld_condition_t;

private:
  l_mutex_ptr d_mutex;

protected:
  inline l_mutex_ptr mutex () { return (d_mutex); };

public:
  __INLINE__ mld_mutex_t () {
#ifdef _USE_OMNI_THREADS_
    d_mutex = new omni_mutex ();
#else
    d_mutex = (l_mutex_ptr) new l_mutex;
    int l_ret = pthread_mutex_init (d_mutex, NULL);
    if (l_ret != 0) {
      fprintf (stderr, "Error %d creating mutex.\n", l_ret);
      throw std::runtime_error ("mld_mutex_t::mld_mutex_t()\n");
    }
#endif
  };

  __INLINE__ ~mld_mutex_t () {
    unlock ();
#ifndef _USE_OMNI_THREADS_
    int l_ret = pthread_mutex_destroy (d_mutex);
    if (l_ret != 0) {
      fprintf (stderr, "mld_mutex_t::~mld_mutex_t(): "
	       "Error %d destroying mutex.\n", l_ret);
    }
#endif
    delete d_mutex;
    d_mutex = NULL;
  };

  __INLINE__ void lock () {
#ifdef _USE_OMNI_THREADS_
    d_mutex->lock ();
#else
    int l_ret = pthread_mutex_lock (d_mutex);
    if (l_ret != 0) {
      fprintf (stderr, "mld_mutex_t::lock(): "
	       "Error %d locking mutex.\n", l_ret);
    }
#endif
  };

  __INLINE__ void unlock () {
#ifdef _USE_OMNI_THREADS_
    d_mutex->unlock ();
#else
    int l_ret = pthread_mutex_unlock (d_mutex);
    if (l_ret != 0) {
      fprintf (stderr, "mld_mutex_t::unlock(): "
	       "Error %d locking mutex.\n", l_ret);
    }
#endif
  };

  __INLINE__ bool trylock () {
#ifdef _USE_OMNI_THREADS_
    int l_ret = d_mutex->trylock ();
#else
    int l_ret = pthread_mutex_unlock (d_mutex);
#endif
    return (l_ret == 0 ? true : false);
  };

  inline void acquire () { lock(); };
  inline void release () { unlock(); };
  inline void wait () { lock(); };
  inline void post () { unlock(); };
};

typedef mld_mutex_t mld_mutex, *mld_mutex_ptr;

class mld_condition_t {
#ifdef _USE_OMNI_THREADS_
  typedef omni_condition l_condition, *l_condition_ptr;
#else
  typedef pthread_cond_t l_condition, *l_condition_ptr;
#endif

private:
  l_condition_ptr d_condition;
  mld_mutex_ptr d_mutex;
  bool d_i_own_mutex;

public:
  __INLINE__ mld_condition_t (mld_mutex_ptr mutex = NULL) {
    if (mutex) {
      d_i_own_mutex = false;
      d_mutex = mutex;
    } else {
      d_i_own_mutex = true;
      d_mutex = new mld_mutex ();
    }
#ifdef _USE_OMNI_THREADS_
    d_condition = new omni_condition (d_mutex->mutex ());
#else
    d_condition = (l_condition_ptr) new l_condition;
    int l_ret = pthread_cond_init (d_condition, NULL);
    if (l_ret != 0) {
      fprintf (stderr, "Error %d creating condition.\n", l_ret);
      throw std::runtime_error ("mld_condition_t::mld_condition_t()\n");
    }
#endif
  };

  __INLINE__ ~mld_condition_t () {
    signal ();
#ifndef _USE_OMNI_THREADS_
    int l_ret = pthread_cond_destroy (d_condition);
    if (l_ret != 0) {
      fprintf (stderr, "mld_condition_t::mld_condition_t(): "
	       "Error %d destroying condition.\n", l_ret);
    }
#endif
    delete d_condition;
    d_condition = NULL;
    if (d_i_own_mutex)
      delete d_mutex;
    d_mutex = NULL;
  };

  __INLINE__ mld_mutex_ptr mutex () {return (d_mutex);};

  __INLINE__ void signal () {
    DEBUG (fprintf (stderr, "a "));

#ifdef _USE_OMNI_THREADS_
    d_condition->signal ();
#else
    int l_ret = pthread_cond_signal (d_condition);
    if (l_ret != 0) {
      fprintf (stderr, "mld_condition_t::signal(): "
	       "Error %d.\n", l_ret);
    }
#endif
    DEBUG (fprintf (stderr, "b "));
  };

  __INLINE__ void wait () {
    DEBUG (fprintf (stderr, "c "));
#ifdef _USE_OMNI_THREADS_
    d_condition->wait ();
#else
    int l_ret = pthread_cond_wait (d_condition, d_mutex->mutex ());
    if (l_ret != 0) {
      fprintf (stderr, "mld_condition_t::wait(): "
	       "Error %d.\n", l_ret);
    }
#endif
    DEBUG (printf (stderr, "d "));
  };
};

typedef mld_condition_t mld_condition, *mld_condition_ptr;

class mld_thread_t {
#ifdef _USE_OMNI_THREADS_
  typedef omni_thread l_thread, *l_thread_ptr;
#else
  typedef pthread_t l_thread, *l_thread_ptr;
#endif

private:
#ifndef _USE_OMNI_THREADS_
  l_thread d_thread;
  void (*d_start_routine)(void*);
  void *d_arg;
#else
  l_thread_ptr d_thread;
#endif

#ifndef _USE_OMNI_THREADS_
  static void* local_start_routine (void *arg) {
    mld_thread_t* This = (mld_thread_t*) arg;
    (*(This->d_start_routine))(This->d_arg);
    return (NULL);
  };
#endif

public:
  __INLINE__ mld_thread_t (void (*start_routine)(void *), void *arg) {
#ifdef _USE_OMNI_THREADS_
    d_thread = new omni_thread (start_routine, arg);
    d_thread->start ();
#else
    d_start_routine = start_routine;
    d_arg = arg;
    int l_ret = pthread_create (&d_thread, NULL, local_start_routine, this);
    if (l_ret != 0) {
      fprintf (stderr, "Error %d creating thread.\n", l_ret);
      throw std::runtime_error ("mld_thread_t::mld_thread_t()\n");
    }
#endif
  };

  __INLINE__ ~mld_thread_t () {
#ifdef _USE_OMNI_THREADS_
//  delete d_thread;
    d_thread = NULL;
#else
    int l_ret = pthread_detach (d_thread);
    if (l_ret != 0) {
      fprintf (stderr, "Error %d detaching thread.\n", l_ret);
      throw std::runtime_error ("mld_thread_t::~mld_thread_t()\n");
    }
#endif
  };
};

typedef mld_thread_t mld_thread, *mld_thread_ptr;

#endif /* _INCLUDED_MLD_THREADS_H_ */
