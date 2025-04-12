/* -*- c++ -*- */
/*
 * Copyright 2009-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_THREAD_H
#define INCLUDED_THREAD_H

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#include <gnuradio/api.h>
#include <boost/thread/barrier.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <memory>
#include <vector>

namespace gr {
namespace thread {

typedef boost::thread thread;
typedef boost::mutex mutex;
typedef boost::unique_lock<boost::mutex> scoped_lock;
typedef boost::condition_variable condition_variable;
typedef boost::barrier barrier;
typedef std::shared_ptr<barrier> barrier_sptr;

/*! \brief a system-dependent typedef for the underlying thread type.
 */
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
typedef HANDLE gr_thread_t;
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
typedef pthread_t gr_thread_t;
#else
typedef pthread_t gr_thread_t;
#endif

/*! \brief Get the current thread's ID as a gr_thread_t
 *
 * We use this when setting the thread affinity or any other
 * low-level thread settings. Can be called within a GNU Radio
 * block to get a reference to its current thread ID.
 */
GR_RUNTIME_API gr_thread_t get_current_thread_id();

/*! \brief Bind the current thread to a set of cores.
 *
 * Wrapper for system-dependent calls to set the affinity of the
 * current thread to the processor mask. The mask is simply a
 * 1-demensional vector containing the processor or core number
 * from 0 to N-1 for N cores.
 *
 * Note: this does not work on OSX; it is a nop call since OSX
 * does not support the concept of thread affinity (and what they
 * do support in this way since 10.5 is not what we want or can
 * use in this fashion).
 */
GR_RUNTIME_API void thread_bind_to_processor(const std::vector<int>& mask);

/*! \brief Convineince function to bind the current thread to a single core.
 *
 * Wrapper for system-dependent calls to set the affinity of the
 * current thread to a given core from 0 to N-1 for N cores.
 *
 * Note: this does not work on OSX; it is a nop call since OSX
 * does not support the concept of thread affinity (and what they
 * do support in this way since 10.5 is not what we want or can
 * use in this fashion).
 */
GR_RUNTIME_API void thread_bind_to_processor(int n);

/*! \brief Bind a thread to a set of cores.
 *
 * Wrapper for system-dependent calls to set the affinity of the
 * given thread ID to the processor mask. The mask is simply a
 * 1-demensional vector containing the processor or core number
 * from 0 to N-1 for N cores.
 *
 * Note: this does not work on OSX; it is a nop call since OSX
 * does not support the concept of thread affinity (and what they
 * do support in this way since 10.5 is not what we want or can
 * use in this fashion).
 */
GR_RUNTIME_API void thread_bind_to_processor(gr_thread_t thread,
                                             const std::vector<int>& mask);


/*! \brief Convineince function to bind the a thread to a single core.
 *
 * Wrapper for system-dependent calls to set the affinity of the
 * given thread ID to a given core from 0 to N-1 for N cores.
 *
 * Note: this does not work on OSX; it is a nop call since OSX
 * does not support the concept of thread affinity (and what they
 * do support in this way since 10.5 is not what we want or can
 * use in this fashion).
 */
GR_RUNTIME_API void thread_bind_to_processor(gr_thread_t thread, unsigned int n);

/*! \brief Remove any thread-processor affinity for the current thread.
 *
 * Note: this does not work on OSX; it is a nop call since OSX
 * does not support the concept of thread affinity (and what they
 * do support in this way since 10.5 is not what we want or can
 * use in this fashion).
 */
GR_RUNTIME_API void thread_unbind();

/*! \brief Remove any thread-processor affinity for a given thread ID.
 *
 * Note: this does not work on OSX; it is a nop call since OSX
 * does not support the concept of thread affinity (and what they
 * do support in this way since 10.5 is not what we want or can
 * use in this fashion).
 */
GR_RUNTIME_API void thread_unbind(gr_thread_t thread);

/*! \brief get current thread priority for a given thread ID
 */
GR_RUNTIME_API int thread_priority(gr_thread_t thread);

/*! \brief set current thread priority for a given thread ID
 */
GR_RUNTIME_API int set_thread_priority(gr_thread_t thread, int priority);

GR_RUNTIME_API void set_thread_name(gr_thread_t thread, std::string name);

} /* namespace thread */
} /* namespace gr */

#endif /* INCLUDED_THREAD_H */
