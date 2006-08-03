//				Package : omnithread
// omnithread/solaris.cc	Created : 7/94 tjr
//
//    Copyright (C) 1994-1999 AT&T Laboratories Cambridge
//
//    This file is part of the omnithread library
//
//    The omnithread library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Library General Public
//    License as published by the Free Software Foundation; either
//    version 2 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Library General Public License for more details.
//
//    You should have received a copy of the GNU Library General Public
//    License along with this library; if not, write to the Free
//    Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
//    02111-1307, USA
//
//
// Implementation of OMNI thread abstraction for solaris threads.
//

#include <stdlib.h>
#include <errno.h>
#include <omnithread.h>

#define DB(x) // x 
// #include <iostream> or #include <iostream.h> if DB is on.

#define THROW_ERRORS(x) { int rc = (x); \
			  if (rc != 0) throw omni_thread_fatal(rc); }



///////////////////////////////////////////////////////////////////////////
//
// Mutex
//
///////////////////////////////////////////////////////////////////////////


omni_mutex::omni_mutex(void)
{
    THROW_ERRORS(mutex_init(&sol_mutex, USYNC_THREAD, 0));
}

omni_mutex::~omni_mutex(void)
{
    THROW_ERRORS(mutex_destroy(&sol_mutex));
}

void
omni_mutex::lock(void)
{
    THROW_ERRORS(mutex_lock(&sol_mutex));
}

void
omni_mutex::unlock(void)
{
    THROW_ERRORS(mutex_unlock(&sol_mutex));
}



///////////////////////////////////////////////////////////////////////////
//
// Condition variable
//
///////////////////////////////////////////////////////////////////////////


omni_condition::omni_condition(omni_mutex* m) : mutex(m)
{
    THROW_ERRORS(cond_init(&sol_cond, USYNC_THREAD, 0));
}

omni_condition::~omni_condition(void)
{
    THROW_ERRORS(cond_destroy(&sol_cond));
}

void
omni_condition::wait(void)
{
    THROW_ERRORS(cond_wait(&sol_cond, &mutex->sol_mutex));
}

int
omni_condition::timedwait(unsigned long secs, unsigned long nanosecs)
{
    timespec rqts = { secs, nanosecs };

 again:
    int rc = cond_timedwait(&sol_cond, &mutex->sol_mutex, &rqts);

    if (rc == 0)
	return 1;

    if (rc == EINTR)
        goto again;

    if (rc == ETIME)
	return 0;

    throw omni_thread_fatal(rc);
}

void
omni_condition::signal(void)
{
    THROW_ERRORS(cond_signal(&sol_cond));
}

void
omni_condition::broadcast(void)
{
    THROW_ERRORS(cond_broadcast(&sol_cond));
}



///////////////////////////////////////////////////////////////////////////
//
// Counting semaphore
//
///////////////////////////////////////////////////////////////////////////


omni_semaphore::omni_semaphore(unsigned int initial)
{
    THROW_ERRORS(sema_init(&sol_sem, initial, USYNC_THREAD, NULL));
}

omni_semaphore::~omni_semaphore(void)
{
    THROW_ERRORS(sema_destroy(&sol_sem));
}

void
omni_semaphore::wait(void)
{
    THROW_ERRORS(sema_wait(&sol_sem));
}

void
omni_semaphore::post(void)
{
    THROW_ERRORS(sema_post(&sol_sem));
}



///////////////////////////////////////////////////////////////////////////
//
// Thread
//
///////////////////////////////////////////////////////////////////////////


//
// Static variables
//

int omni_thread::init_t::count = 0;

omni_mutex* omni_thread::next_id_mutex;
int omni_thread::next_id = 0;

static thread_key_t self_key;

static size_t stack_size = 0;

//
// Initialisation function (gets called before any user code).
//

omni_thread::init_t::init_t(void)
{
    if (count++ != 0)	// only do it once however many objects get created.
	return;

    DB(cerr << "omni_thread::init: solaris implementation initialising\n");

    THROW_ERRORS(thr_keycreate(&self_key, NULL));

    next_id_mutex = new omni_mutex;

    //
    // Create object for this (i.e. initial) thread.
    //

    omni_thread* t = new omni_thread;

    t->_state = STATE_RUNNING;

    t->sol_thread = thr_self();

    DB(cerr << "initial thread " << t->id() << " sol_thread " << t->sol_thread
       << endl);

    THROW_ERRORS(thr_setspecific(self_key, (void*)t));

    THROW_ERRORS(thr_setprio(t->sol_thread, sol_priority(PRIORITY_NORMAL)));
}


//
// Wrapper for thread creation.
//

extern "C" void*
omni_thread_wrapper(void* ptr)
{
    omni_thread* me = (omni_thread*)ptr;

    DB(cerr << "omni_thread::wrapper: thread " << me->id()
       << " started\n");

    THROW_ERRORS(thr_setspecific(self_key, me));

    //
    // Now invoke the thread function with the given argument.
    //

    if (me->fn_void != NULL) {
	(*me->fn_void)(me->thread_arg);
	omni_thread::exit();
    }

    if (me->fn_ret != NULL) {
	void* return_value = (*me->fn_ret)(me->thread_arg);
	omni_thread::exit(return_value);
    }

    if (me->detached) {
	me->run(me->thread_arg);
	omni_thread::exit();
    } else {
	void* return_value = me->run_undetached(me->thread_arg);
	omni_thread::exit(return_value);
    }

    // should never get here.

    return NULL;
}


//
// Constructors for omni_thread - set up the thread object but don't
// start it running.
//

// construct a detached thread running a given function.

omni_thread::omni_thread(void (*fn)(void*), void* arg, priority_t pri)
{
    common_constructor(arg, pri, 1);
    fn_void = fn;
    fn_ret = NULL;
}

// construct an undetached thread running a given function.

omni_thread::omni_thread(void* (*fn)(void*), void* arg, priority_t pri)
{
    common_constructor(arg, pri, 0);
    fn_void = NULL;
    fn_ret = fn;
}

// construct a thread which will run either run() or run_undetached().

omni_thread::omni_thread(void* arg, priority_t pri)
{
    common_constructor(arg, pri, 1);
    fn_void = NULL;
    fn_ret = NULL;
}

// common part of all constructors.

void
omni_thread::common_constructor(void* arg, priority_t pri, int det)
{
    _state = STATE_NEW;
    _priority = pri;

    next_id_mutex->lock();
    _id = next_id++;
    next_id_mutex->unlock();

    thread_arg = arg;
    detached = det;	// may be altered in start_undetached()

    _dummy       = 0;
    _values      = 0;
    _value_alloc = 0;
    // sol_thread is set up in initialisation routine or start().
}


//
// Destructor for omni_thread.
//

omni_thread::~omni_thread(void)
{
    DB(cerr << "destructor called for thread " << id() << endl);
    if (_values) {
        for (key_t i=0; i < _value_alloc; i++) {
	    if (_values[i]) {
	        delete _values[i];
	    }
        }
	delete [] _values;
    }
}


//
// Start the thread
//

void
omni_thread::start(void)
{
    long flags = 0;

    if (detached)
	flags |= THR_DETACHED;

    omni_mutex_lock l(mutex);

    if (_state != STATE_NEW)
	throw omni_thread_invalid();

    THROW_ERRORS(thr_create(0, stack_size, omni_thread_wrapper, (void*)this, flags,
			    &sol_thread));

    _state = STATE_RUNNING;

    THROW_ERRORS(thr_setprio(sol_thread, sol_priority(_priority)));
}


//
// Start a thread which will run the member function run_undetached().
//

void
omni_thread::start_undetached(void)
{
    if ((fn_void != NULL) || (fn_ret != NULL))
	throw omni_thread_invalid();

    detached = 0;
    start();
}


//
// join - simply check error conditions & call thr_join.
//

void
omni_thread::join(void** status)
{
    mutex.lock();

    if ((_state != STATE_RUNNING) && (_state != STATE_TERMINATED)) {
	mutex.unlock();
	throw omni_thread_invalid();
    }

    mutex.unlock();

    if (this == self())
	throw omni_thread_invalid();

    if (detached)
	throw omni_thread_invalid();

    DB(cerr << "omni_thread::join: doing thr_join\n");

    THROW_ERRORS(thr_join(sol_thread, (thread_t *)NULL, status));

    DB(cerr << "omni_thread::join: thr_join succeeded\n");

    delete this;
}


//
// Change this thread's priority.
//

void
omni_thread::set_priority(priority_t pri)
{
    omni_mutex_lock l(mutex);

    if (_state != STATE_RUNNING)
	throw omni_thread_invalid();

    _priority = pri;

    THROW_ERRORS(thr_setprio(sol_thread, sol_priority(pri)));
}


//
// create - construct a new thread object and start it running.  Returns thread
// object if successful, null pointer if not.
//

// detached version

omni_thread*
omni_thread::create(void (*fn)(void*), void* arg, priority_t pri)
{
    omni_thread* t = new omni_thread(fn, arg, pri);

    t->start();

    return t;
}

// undetached version

omni_thread*
omni_thread::create(void* (*fn)(void*), void* arg, priority_t pri)
{
    omni_thread* t = new omni_thread(fn, arg, pri);

    t->start();

    return t;
}


//
// exit() _must_ lock the mutex even in the case of a detached thread.  This is
// because a thread may run to completion before the thread that created it has
// had a chance to get out of start().  By locking the mutex we ensure that the
// creating thread must have reached the end of start() before we delete the
// thread object.  Of course, once the call to start() returns, the user can
// still incorrectly refer to the thread object, but that's their problem.
//

void
omni_thread::exit(void* return_value)
{
    omni_thread* me = self();

    if (me)
      {
	me->mutex.lock();

	me->_state = STATE_TERMINATED;

	me->mutex.unlock();

	DB(cerr << "omni_thread::exit: thread " << me->id() << " detached "
	   << me->detached << " return value " << return_value << endl);

	if (me->detached)
	  delete me;
      }
    else
      {
	DB(cerr << "omni_thread::exit: called with a non-omnithread. Exit quietly." << endl);
      }

    thr_exit(return_value);
}


omni_thread*
omni_thread::self(void)
{
    omni_thread* me;

    THROW_ERRORS(thr_getspecific(self_key, (void**)&me));

    if (!me) {
      // This thread is not created by omni_thread::start because it
      // doesn't has a class omni_thread instance attached to its key.
      DB(cerr << "omni_thread::self: called with a non-ominthread. NULL is returned." << endl);
    }

    return me;
}


void
omni_thread::yield(void)
{
    thr_yield();
}


void
omni_thread::sleep(unsigned long secs, unsigned long nanosecs)
{
    timespec rqts = { secs, nanosecs };
    timespec remain;
    while (nanosleep(&rqts, &remain)) {
      if (errno == EINTR) {
	rqts.tv_sec  = remain.tv_sec;
	rqts.tv_nsec = remain.tv_nsec;
	continue;
      }
      else
	throw omni_thread_fatal(errno);
    }
}


void
omni_thread::get_time(unsigned long* abs_sec, unsigned long* abs_nsec,
		      unsigned long rel_sec, unsigned long rel_nsec)
{
    timespec abs;
    clock_gettime(CLOCK_REALTIME, &abs);
    abs.tv_nsec += rel_nsec;
    abs.tv_sec += rel_sec + abs.tv_nsec / 1000000000;
    abs.tv_nsec = abs.tv_nsec % 1000000000;
    *abs_sec = abs.tv_sec;
    *abs_nsec = abs.tv_nsec;
}


int
omni_thread::sol_priority(priority_t pri)
{
    switch (pri) {

    case PRIORITY_LOW:
	return 0;

    case PRIORITY_NORMAL:
	return 1;

    case PRIORITY_HIGH:
	return 2;
    }

    throw omni_thread_invalid();
}


void
omni_thread::stacksize(unsigned long sz)
{
  stack_size = sz;
}

unsigned long
omni_thread::stacksize()
{
  return stack_size;
}


//
// Dummy thread
//

#error This dummy thread code is not tested. It might work if you're lucky.

class omni_thread_dummy : public omni_thread {
public:
  inline omni_thread_dummy() : omni_thread()
  {
    _dummy = 1;
    _state = STATE_RUNNING;
    sol_thread = thr_self();
    THROW_ERRORS(thr_setspecific(self_key, (void*)this));
  }
  inline ~omni_thread_dummy()
  {
    THROW_ERRORS(thr_setspecific(self_key, 0));
  }
};

omni_thread*
omni_thread::create_dummy()
{
  if (omni_thread::self())
    throw omni_thread_invalid();

  return new omni_thread_dummy;
}

void
omni_thread::release_dummy()
{
  omni_thread* self = omni_thread::self();
  if (!self || !self->_dummy)
    throw omni_thread_invalid();

  omni_thread_dummy* dummy = (omni_thread_dummy*)self;
  delete dummy;
}


#define INSIDE_THREAD_IMPL_CC
#include "threaddata.cc"
#undef INSIDE_THREAD_IMPL_CC
