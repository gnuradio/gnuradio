//				Package : omnithread
// omnithread/mach.cc		Created : 7/97 lars immisch lars@ibp.de
//
//    Copyright (C) 1997 Immisch, Becker & Partner
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
// Implementation of OMNI thread abstraction for mach threads
//
// to the author's pleasure, mach cthreads are very similar to posix threads
//

#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <mach/cthreads.h>
#include "omnithread.h"

#define DB(x) // x
// #include <iostream> or #include <iostream.h> if DB is on.

#define ERRNO(x) (x)

//
// static variables
//

int omni_thread::init_t::count = 0;

omni_mutex* omni_thread::next_id_mutex;
int omni_thread::next_id = 0;

static int normal_priority;
static int highest_priority;

static size_t stack_size = 0;

///////////////////////////////////////////////////////////////////////////
//
// Mutex
//
///////////////////////////////////////////////////////////////////////////


omni_mutex::omni_mutex(void)
{
  mutex_init(&mach_mutex);	
}


omni_mutex::~omni_mutex(void)
{
  mutex_clear(&mach_mutex);	
}


void omni_mutex::lock(void)
{
  mutex_lock(&mach_mutex);
}


void omni_mutex::unlock(void)
{
  mutex_unlock(&mach_mutex);
}



///////////////////////////////////////////////////////////////////////////
//
// Condition variable
//
///////////////////////////////////////////////////////////////////////////


omni_condition::omni_condition(omni_mutex* m) : mutex(m)
{
  condition_init(&mach_cond);
}


omni_condition::~omni_condition(void)
{
  condition_clear(&mach_cond);
}

void
omni_condition::wait(void)
{
  condition_wait(&mach_cond, &mutex->mach_mutex);
}

typedef struct alarmclock_args {
  unsigned long secs;
  unsigned long nsecs;
  bool wakeup;
  condition_t condition;
  mutex_t mutex;
};

any_t alarmclock(any_t arg)
{
  alarmclock_args* alarm = (alarmclock_args*)arg;
	
  omni_thread::sleep(alarm->secs, alarm->nsecs);

  mutex_lock(alarm->mutex);
			
  alarm->wakeup = TRUE;
	
  condition_signal(alarm->condition);
		
  mutex_unlock(alarm->mutex);

  return (any_t)TRUE;
}

int omni_condition::timedwait(unsigned long abs_secs, unsigned long abs_nsecs)
{
  alarmclock_args alarm;

  omni_thread::get_time(&alarm.secs, &alarm.nsecs, 0, 0);
	
  if (abs_secs < alarm.secs || (abs_secs == alarm.secs && abs_nsecs <= alarm.nsecs))
    return ETIMEDOUT;

  alarm.secs = abs_secs - alarm.secs;
  if (abs_nsecs <= alarm.nsecs) {
    alarm.nsecs = 1000000 - alarm.nsecs + abs_nsecs;
    alarm.secs--;
  }
  else {
    alarm.nsecs = abs_nsecs - alarm.nsecs;
  }

  alarm.mutex = &mutex->mach_mutex;
  alarm.condition = &mach_cond;
  alarm.wakeup = FALSE;
	
  cthread_t ct = cthread_fork((cthread_fn_t)alarmclock, (any_t)&alarm);
  cthread_detach(ct);
	
  condition_wait(&mach_cond, &mutex->mach_mutex);
		
  if (alarm.wakeup) {
    return 0;
  }
	
  // interrupt the alarmclock thread sleep
  cthread_abort(ct);
	
  // wait until it has signalled the condition
  condition_wait(&mach_cond, &mutex->mach_mutex);

  return 1;
}


void omni_condition::signal(void)
{
  condition_signal(&mach_cond);
}


void omni_condition::broadcast(void)
{
  condition_signal(&mach_cond);
}



///////////////////////////////////////////////////////////////////////////
//
// Counting semaphore
//
///////////////////////////////////////////////////////////////////////////


omni_semaphore::omni_semaphore(unsigned int initial) : c(&m)
{
  value = initial;
}


omni_semaphore::~omni_semaphore(void)
{
}


void 
omni_semaphore::wait(void)
{
  omni_mutex_lock l(m);

  while (value == 0)
    c.wait();

  value--;
}


int
omni_semaphore::trywait(void)
{
  omni_mutex_lock l(m);

  if (value == 0)
    return 0;

  value--;
  return 1;
}


void
omni_semaphore::post(void)
{
  omni_mutex_lock l(m);

  if (value == 0)
    c.signal();

  value++;
}



///////////////////////////////////////////////////////////////////////////
//
// Thread
//
///////////////////////////////////////////////////////////////////////////



//
// Initialisation function (gets called before any user code).
//

omni_thread::init_t::init_t(void)
{
  if (count++ != 0)	// only do it once however many objects get created.
    return;

  //
  // find base and max priority. 
  // This is the initial thread, so the max priority of this
  // thread also applies to any newly created thread.
  //

  kern_return_t				error;
  struct thread_sched_info	info;
  unsigned int				info_count = THREAD_SCHED_INFO_COUNT;

  error = thread_info(thread_self(), THREAD_SCHED_INFO, (thread_info_t)&info, &info_count);
  if (error != KERN_SUCCESS) {
    DB(cerr << "omni_thread::init: error determining thread_info" << endl);
    ::exit(1);
  }
  else {
    normal_priority = info.base_priority;
    highest_priority = info.max_priority;
  }

  next_id_mutex = new omni_mutex;

  //
  // Create object for this (i.e. initial) thread.
  //

  omni_thread* t = new omni_thread;

  if (t->_state != STATE_NEW) {
    DB(cerr << "omni_thread::init: problem creating initial thread object\n");
    ::exit(1);
  }

  t->_state = STATE_RUNNING;
  
  t->mach_thread = cthread_self();

  DB(cerr << "initial thread " << t->id() << endl);

  cthread_set_data(t->mach_thread, (any_t)t);
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

  cthread_set_data(cthread_self(), (any_t)me);

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

void omni_thread::common_constructor(void* arg, priority_t pri, int det)
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
  // posix_thread is set up in initialisation routine or start().
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
  omni_mutex_lock l(mutex);

  int rc;

  if (_state != STATE_NEW)
    throw omni_thread_invalid();

  mach_thread = cthread_fork(omni_thread_wrapper, (any_t)this);
	
  _state = STATE_RUNNING;

  if (detached) {
    cthread_detach(mach_thread);
  }
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
// join - simply check error conditions & call cthread_join.
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

  DB(cerr << "omni_thread::join: doing cthread_join\n");

 *status = cthread_join(mach_thread);

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

  kern_return_t rc = cthread_priority(mach_thread, mach_priority(pri), FALSE);
	
  if (rc != KERN_SUCCESS)
    throw omni_thread_fatal(errno);
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

void omni_thread::exit(void* return_value)
{
  omni_thread* me = self();

  if (me)
    {
      me->mutex.lock();

      if (me->_state != STATE_RUNNING)
	DB(cerr << "omni_thread::exit: thread not in \"running\" state\n");

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
  cthread_exit(return_value);
}

omni_thread* omni_thread::self(void)
{
  omni_thread* me;

  me = (omni_thread*)cthread_data(cthread_self());

  if (!me) {
    // This thread is not created by omni_thread::start because it
    // doesn't has a class omni_thread instance attached to its key.
    DB(cerr << "omni_thread::self: called with a non-ominthread. NULL is returned." << endl);
  }
	
  return me;
}

void omni_thread::yield(void)
{
  cthread_yield();
}

#define MAX_SLEEP_SECONDS (unsigned)4294966	// (2**32-2)/1000

void
omni_thread::sleep(unsigned long secs, unsigned long nanosecs)
{
  if (secs <= MAX_SLEEP_SECONDS) {
    thread_switch(THREAD_NULL, SWITCH_OPTION_WAIT, secs * 1000 + nanosecs / 1000000);
    return;
  }

  unsigned no_of_max_sleeps = secs / MAX_SLEEP_SECONDS;

  for (unsigned i = 0; i < no_of_max_sleeps; i++)
    thread_switch(THREAD_NULL, SWITCH_OPTION_WAIT, MAX_SLEEP_SECONDS * 1000);

  thread_switch(THREAD_NULL, SWITCH_OPTION_WAIT, 
		(secs % MAX_SLEEP_SECONDS) * 1000 + nanosecs / 1000000);
	
  return;
}

void
omni_thread::get_time(unsigned long* abs_sec, unsigned long* abs_nsec,
		      unsigned long rel_sec, unsigned long rel_nsec)
{
  int rc;
  unsigned long tv_sec;
  unsigned long tv_nsec;
  struct timeval tv;
	
  rc = gettimeofday(&tv, NULL); 
  if (rc)	throw omni_thread_fatal(rc);

  tv_sec = tv.tv_sec;
  tv_nsec = tv.tv_usec * 1000;

  tv_nsec += rel_nsec;
  tv_sec += rel_sec + tv_nsec / 1000000000;
  tv_nsec = tv_nsec % 1000000000;

  *abs_sec = tv_sec;
  *abs_nsec = tv_nsec;
}


int 
omni_thread::mach_priority(priority_t pri)
{
  switch (pri) {

  case PRIORITY_LOW:
    return 0;

  case PRIORITY_NORMAL:
    return normal_priority;

  case PRIORITY_HIGH:
    return highest_priority;

  default:
    return -1;
  }
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
    mach_thread = cthread_self();
    cthread_set_data(mach_thread, (any_t)this));
  }
  inline ~omni_thread_dummy()
  {
    cthread_set_data(mach_thread, (any_t)0));
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
