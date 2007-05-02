//				Package : omnithread
// omnithread/posix.cc		Created : 7/94 tjr
//
//    Copyright (C) 2006 Free Software Foundation, Inc.
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
//    Software Foundation, Inc., 51 Franklin Street, Boston, MA  
//    02110-1301, USA
//

//
// Implementation of OMNI thread abstraction for posix threads
//
// The source below tests for the definition of the macros:
//     PthreadDraftVersion
//     PthreadSupportThreadPriority
//     NoNanoSleep
//     NeedPthreadInit
//
// As different draft versions of the pthread standard P1003.4a/P1003.1c
// define slightly different APIs, the macro 'PthreadDraftVersion'
// identifies the draft version supported by this particular platform.
//
// Some unix variants do not support thread priority unless a real-time
// kernel option is installed. The macro 'PthreadSupportThreadPriority',
// if defined, enables the use of thread priority. If it is not defined,
// setting or changing thread priority will be silently ignored.
//
// nanosleep() is defined in Posix P1003.4 since Draft 9 (?).
// Not all platforms support this standard. The macro 'NoNanoSleep'
// identifies platform that don't.
//

#include <config.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <omnithread.h>

#if (PthreadDraftVersion == 0)
#error "PthreadDraftVersion not defined.  If not sure, define it to 10"
#endif

#ifdef HAVE_NANOSLEEP
#undef NoNanoSleep
#else
#define NoNanoSleep
#endif

#ifdef HAVE_SYS_TIME_H
// typedef of struct timeval and gettimeofday();
#include <sys/time.h>
#include <unistd.h>
#endif

#if defined(__linux__) && defined(_MIT_POSIX_THREADS)
#include <pthread/mit/sys/timers.h>
#endif

#if defined(__irix__) && defined(PthreadSupportThreadPriority)
#if _POSIX_THREAD_PRIORITY_SCHEDULING
#include <sched.h>
#endif
#endif

#if 1
#define DB(x) // x
#else
#define DB(x) x
#include <iostream>
using std::cerr;
using std::endl;
#endif

#if (PthreadDraftVersion <= 6)
#define ERRNO(x) (((x) != 0) ? (errno) : 0)
#ifdef __VMS
// pthread_setprio returns old priority on success (draft version 4:
// OpenVms version < 7)
#define THROW_ERRORS(x) { if ((x) == -1) throw omni_thread_fatal(errno); }
#else
#define THROW_ERRORS(x) { if ((x) != 0) throw omni_thread_fatal(errno); }
#endif
#else
#define ERRNO(x) (x)
#define THROW_ERRORS(x) { int rc = (x); \
			  if (rc != 0) throw omni_thread_fatal(rc); }
#endif



///////////////////////////////////////////////////////////////////////////
//
// Mutex
//
///////////////////////////////////////////////////////////////////////////


omni_mutex::omni_mutex(void)
{
#if (PthreadDraftVersion == 4)
    THROW_ERRORS(pthread_mutex_init(&posix_mutex, pthread_mutexattr_default));
#else
    THROW_ERRORS(pthread_mutex_init(&posix_mutex, 0));
#endif
}

omni_mutex::~omni_mutex(void)
{
    THROW_ERRORS(pthread_mutex_destroy(&posix_mutex));
}


///////////////////////////////////////////////////////////////////////////
//
// Condition variable
//
///////////////////////////////////////////////////////////////////////////


omni_condition::omni_condition(omni_mutex* m) : mutex(m)
{
#if (PthreadDraftVersion == 4)
    THROW_ERRORS(pthread_cond_init(&posix_cond, pthread_condattr_default));
#else
    THROW_ERRORS(pthread_cond_init(&posix_cond, 0));
#endif
}

omni_condition::~omni_condition(void)
{
    THROW_ERRORS(pthread_cond_destroy(&posix_cond));
}

void
omni_condition::wait(void)
{
    THROW_ERRORS(pthread_cond_wait(&posix_cond, &mutex->posix_mutex));
}

int
omni_condition::timedwait(unsigned long secs, unsigned long nanosecs)
{
    timespec rqts = { secs, nanosecs };

again:
    int rc = ERRNO(pthread_cond_timedwait(&posix_cond,
					  &mutex->posix_mutex, &rqts));
    if (rc == 0)
	return 1;

#if (PthreadDraftVersion <= 6)
    if (rc == EAGAIN)
	return 0;
#endif

    // Some versions of unix produces this errno when the wait was
    // interrupted by a unix signal or fork.
    // Some versions of the glibc 2.0.x produces this errno when the 
    // program is debugged under gdb. Straightly speaking this is non-posix
    // compliant. We catch this here to make debugging possible.
    if (rc == EINTR)
      goto again;

    if (rc == ETIMEDOUT)
	return 0;

    throw omni_thread_fatal(rc);
#ifdef _MSC_VER
    return 0;
#endif
}

void
omni_condition::signal(void)
{
    THROW_ERRORS(pthread_cond_signal(&posix_cond));
}

void
omni_condition::broadcast(void)
{
    THROW_ERRORS(pthread_cond_broadcast(&posix_cond));
}



///////////////////////////////////////////////////////////////////////////
//
// Counting (or binary) semaphore
//
///////////////////////////////////////////////////////////////////////////


omni_semaphore::omni_semaphore(unsigned int initial, unsigned int _max_count) : c(&m)
{
    value = initial;
    max_count = _max_count;
    if (value < 0 || max_count < 1)
      throw omni_thread_fatal(0);
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
    {
        omni_mutex_lock l(m);
	if (value < max_count)
	  value++;
    }

    c.signal();
}



///////////////////////////////////////////////////////////////////////////
//
// Thread
//
///////////////////////////////////////////////////////////////////////////


//
// static variables
//

omni_mutex* omni_thread::next_id_mutex;
int omni_thread::next_id = 0;

static pthread_key_t self_key;

#ifdef PthreadSupportThreadPriority
static int lowest_priority;
static int normal_priority;
static int highest_priority;
#endif

#if defined(__osf1__) && defined(__alpha__) || defined(__VMS)
// omniORB requires a larger stack size than the default (21120) on OSF/1
static size_t stack_size = 32768;
#elif defined(__rtems__)
static size_t stack_size = ThreadStackSize;
#elif defined(__aix__)
static size_t stack_size = 262144;
#else
static size_t stack_size = 0;
#endif

//
// Initialisation function (gets called before any user code).
//

static int& count() {
  static int the_count = 0;
  return the_count;
}

omni_thread::init_t::init_t(void)
{
    if (count()++ != 0)	// only do it once however many objects get created.
	return;

    DB(cerr << "omni_thread::init: posix 1003.4a/1003.1c (draft "
       << PthreadDraftVersion << ") implementation initialising\n");

#ifdef NeedPthreadInit

    pthread_init();

#endif

#if (PthreadDraftVersion == 4)
    THROW_ERRORS(pthread_keycreate(&self_key, NULL));
#else
    THROW_ERRORS(pthread_key_create(&self_key, NULL));
#endif

#ifdef PthreadSupportThreadPriority

#if defined(__osf1__) && defined(__alpha__) || defined(__VMS)

    lowest_priority = PRI_OTHER_MIN;
    highest_priority = PRI_OTHER_MAX;

#elif defined(__hpux__)

    lowest_priority = PRI_OTHER_MIN;
    highest_priority = PRI_OTHER_MAX;

#elif defined(__sunos__) && (__OSVERSION__ == 5)

    // a bug in pthread_attr_setschedparam means lowest priority is 1 not 0

    lowest_priority  = 1;
    highest_priority = 3;

#else

    lowest_priority = sched_get_priority_min(SCHED_FIFO);
    highest_priority = sched_get_priority_max(SCHED_FIFO);

#endif

    switch (highest_priority - lowest_priority) {

    case 0:
    case 1:
	normal_priority = lowest_priority;
	break;

    default:
	normal_priority = lowest_priority + 1;
	break;
    }

#endif   /* PthreadSupportThreadPriority */

    next_id_mutex = new omni_mutex;

    //
    // Create object for this (i.e. initial) thread.
    //

    omni_thread* t = new omni_thread;

    t->_state = STATE_RUNNING;

    t->posix_thread = pthread_self ();

    DB(cerr << "initial thread " << t->id() << endl);

    THROW_ERRORS(pthread_setspecific(self_key, (void*)t));

#ifdef PthreadSupportThreadPriority

#if (PthreadDraftVersion == 4)

    THROW_ERRORS(pthread_setprio(t->posix_thread,
				 posix_priority(PRIORITY_NORMAL)));

#elif (PthreadDraftVersion == 6)

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    THROW_ERRORS(pthread_attr_setprio(&attr, posix_priority(PRIORITY_NORMAL)));

    THROW_ERRORS(pthread_setschedattr(t->posix_thread, attr));

#else

    struct sched_param sparam;

    sparam.sched_priority = posix_priority(PRIORITY_NORMAL);

    THROW_ERRORS(pthread_setschedparam(t->posix_thread, SCHED_OTHER, &sparam));

#endif   /* PthreadDraftVersion */

#endif   /* PthreadSupportThreadPriority */
}

omni_thread::init_t::~init_t(void)
{
    if (--count() != 0) return;

    omni_thread* self = omni_thread::self();
    if (!self) return;

    pthread_setspecific(self_key, 0);
    delete self;

    delete next_id_mutex;
}

//
// Wrapper for thread creation.
//

extern "C" void* 
omni_thread_wrapper(void* ptr)
{
    omni_thread* me = (omni_thread*)ptr;

    DB(cerr << "omni_thread_wrapper: thread " << me->id()
       << " started\n");

    THROW_ERRORS(pthread_setspecific(self_key, me));

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

    if (_state != STATE_NEW)
	throw omni_thread_invalid();

    pthread_attr_t attr;

#if (PthreadDraftVersion == 4)
    pthread_attr_create(&attr);
#else
    pthread_attr_init(&attr);
#endif

#if (PthreadDraftVersion == 8)
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_UNDETACHED);
#endif

#ifdef PthreadSupportThreadPriority

#if (PthreadDraftVersion <= 6)

    THROW_ERRORS(pthread_attr_setprio(&attr, posix_priority(_priority)));

#else

    struct sched_param sparam;

    sparam.sched_priority = posix_priority(_priority);

    THROW_ERRORS(pthread_attr_setschedparam(&attr, &sparam));

#endif	/* PthreadDraftVersion */

#endif	/* PthreadSupportThreadPriority */

#if !defined(__linux__)
    if (stack_size) {
      THROW_ERRORS(pthread_attr_setstacksize(&attr, stack_size));
    }
#endif


#if (PthreadDraftVersion == 4)
    THROW_ERRORS(pthread_create(&posix_thread, attr, omni_thread_wrapper,
				(void*)this));
    pthread_attr_delete(&attr);
#else
    THROW_ERRORS(pthread_create(&posix_thread, &attr, omni_thread_wrapper,
				(void*)this));
    pthread_attr_destroy(&attr);
#endif

    _state = STATE_RUNNING;

    if (detached) {

#if (PthreadDraftVersion <= 6)
	THROW_ERRORS(pthread_detach(&posix_thread));
#else
	THROW_ERRORS(pthread_detach(posix_thread));
#endif
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
// join - simply check error conditions & call pthread_join.
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

    DB(cerr << "omni_thread::join: doing pthread_join\n");

    THROW_ERRORS(pthread_join(posix_thread, status));

    DB(cerr << "omni_thread::join: pthread_join succeeded\n");

#if (PthreadDraftVersion == 4)
    // With draft 4 pthreads implementations (HPUX 10.x and
    // Digital Unix 3.2), have to detach the thread after 
    // join. If not, the storage for the thread will not be
    // be reclaimed.
    THROW_ERRORS(pthread_detach(&posix_thread));
#endif

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

#ifdef PthreadSupportThreadPriority

#if (PthreadDraftVersion == 4)

    THROW_ERRORS(pthread_setprio(posix_thread, posix_priority(pri)));

#elif (PthreadDraftVersion == 6)

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    THROW_ERRORS(pthread_attr_setprio(&attr, posix_priority(pri)));

    THROW_ERRORS(pthread_setschedattr(posix_thread, attr));

#else

    struct sched_param sparam;

    sparam.sched_priority = posix_priority(pri);

    THROW_ERRORS(pthread_setschedparam(posix_thread, SCHED_OTHER, &sparam));

#endif   /* PthreadDraftVersion */

#endif   /* PthreadSupportThreadPriority */
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

    pthread_exit(return_value);
}


omni_thread*
omni_thread::self(void)
{
    omni_thread* me;

#if (PthreadDraftVersion <= 6)

    THROW_ERRORS(pthread_getspecific(self_key, (void**)&me));

#else

    me = (omni_thread *)pthread_getspecific(self_key);

#endif

    if (!me) {
      // This thread is not created by omni_thread::start because it
      // doesn't has a class omni_thread instance attached to its key.
      DB(cerr << "omni_thread::self: called with a non-omnithread. NULL is returned." << endl);
    }

    return me;
}


void
omni_thread::yield(void)
{
#if (PthreadDraftVersion == 6)

    pthread_yield(NULL);

#elif (PthreadDraftVersion < 9)

    pthread_yield();

#else

    THROW_ERRORS(sched_yield());

#endif
}


void
omni_thread::sleep(unsigned long secs, unsigned long nanosecs)
{
    timespec rqts = { secs, nanosecs };

#ifndef NoNanoSleep

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
#else

#if defined(__osf1__) && defined(__alpha__) || defined(__hpux__) && (__OSVERSION__ == 10) || defined(__VMS) || defined(__SINIX__) || defined (__POSIX_NT__)

    if (pthread_delay_np(&rqts) != 0)
	throw omni_thread_fatal(errno);

#elif defined(__linux__) || defined(__aix__)

    if (secs > 2000) {
      while ((secs = ::sleep(secs))) ;
    } else {
	usleep(secs * 1000000 + (nanosecs / 1000));
    }

#elif defined(__darwin__) || defined(__macos__)

    // Single UNIX Specification says argument of usleep() must be
    // less than 1,000,000.
    secs += nanosecs / 1000000000;
    nanosecs %= 1000000000;
    while ((secs = ::sleep(secs))) ;
    usleep(nanosecs / 1000);

#else

    throw omni_thread_invalid();

#endif
#endif	/* NoNanoSleep */
}


void
omni_thread::get_time(unsigned long* abs_sec, unsigned long* abs_nsec,
		      unsigned long rel_sec, unsigned long rel_nsec)
{
    timespec abs;

#if defined(__osf1__) && defined(__alpha__) || defined(__hpux__) && (__OSVERSION__ == 10) || defined(__VMS) || defined(__SINIX__) || defined(__POSIX_NT__)

    timespec rel;
    rel.tv_sec = rel_sec;
    rel.tv_nsec = rel_nsec;
    THROW_ERRORS(pthread_get_expiration_np(&rel, &abs));

#else

#ifdef HAVE_CLOCK_GETTIME	/* __linux__ || __aix__ */

    clock_gettime(CLOCK_REALTIME, &abs);

#elif defined(HAVE_GETTIMEOFDAY)	/* defined(__linux__) || defined(__aix__) || defined(__SCO_VERSION__) || defined(__darwin__) || defined(__macos__) */

    struct timeval tv;
    gettimeofday(&tv, NULL); 
    abs.tv_sec = tv.tv_sec;
    abs.tv_nsec = tv.tv_usec * 1000;

#else
#error no get time support
#endif	/* __linux__ || __aix__ */

    abs.tv_nsec += rel_nsec;
    abs.tv_sec += rel_sec + abs.tv_nsec / 1000000000;
    abs.tv_nsec = abs.tv_nsec % 1000000000;

#endif	/* __osf1__ && __alpha__ */

    *abs_sec = abs.tv_sec;
    *abs_nsec = abs.tv_nsec;
}


int
omni_thread::posix_priority(priority_t pri)
{
#ifdef PthreadSupportThreadPriority
    switch (pri) {

    case PRIORITY_LOW:
	return lowest_priority;

    case PRIORITY_NORMAL:
	return normal_priority;

    case PRIORITY_HIGH:
	return highest_priority;

    }
#endif

    throw omni_thread_invalid();
#ifdef _MSC_VER
    return 0;
#endif
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

class omni_thread_dummy : public omni_thread {
public:
  inline omni_thread_dummy() : omni_thread()
  {
    _dummy = 1;
    _state = STATE_RUNNING;
    posix_thread = pthread_self();
    THROW_ERRORS(pthread_setspecific(self_key, (void*)this));
  }
  inline ~omni_thread_dummy()
  {
    THROW_ERRORS(pthread_setspecific(self_key, 0));
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
