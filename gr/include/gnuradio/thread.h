/* -*- c++ -*- */
/*
 * Copyright 2009-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/api.h>
#include <memory>
#include <vector>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#endif

namespace gr {
namespace thread {

/*! \brief a system-dependent typedef for the underlying thread type.
 */
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
using gr_thread_t = HANDLE;
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
using gr_thread_t = pthread_t;
#else
using gr_thread_t = pthread_t;
#endif

/*! \brief Get the current thread's ID as a gr_thread_t
 *
 * We use this when setting the thread affinity or any other
 * low-level thread settings. Can be called within a GNU Radio
 * block to get a reference to its current thread ID.
 */
gr_thread_t GR_RUNTIME_API get_current_thread_id();

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
void GR_RUNTIME_API thread_bind_to_processor(const std::vector<unsigned int>& mask);

/*! \brief Convenience function to bind the current thread to a single core.
 *
 * Wrapper for system-dependent calls to set the affinity of the
 * current thread to a given core from 0 to N-1 for N cores.
 *
 * Note: this does not work on OSX; it is a nop call since OSX
 * does not support the concept of thread affinity (and what they
 * do support in this way since 10.5 is not what we want or can
 * use in this fashion).
 */
void GR_RUNTIME_API thread_bind_to_processor(unsigned int n);

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
void GR_RUNTIME_API thread_bind_to_processor(gr_thread_t thread,
                                             const std::vector<unsigned int>& mask);


/*! \brief Convenience function to bind the a thread to a single core.
 *
 * Wrapper for system-dependent calls to set the affinity of the
 * given thread ID to a given core from 0 to N-1 for N cores.
 *
 * Note: this does not work on OSX; it is a nop call since OSX
 * does not support the concept of thread affinity (and what they
 * do support in this way since 10.5 is not what we want or can
 * use in this fashion).
 */
void GR_RUNTIME_API thread_bind_to_processor(gr_thread_t thread, unsigned int n);

/*! \brief Remove any thread-processor affinity for the current thread.
 *
 * Note: this does not work on OSX; it is a nop call since OSX
 * does not support the concept of thread affinity (and what they
 * do support in this way since 10.5 is not what we want or can
 * use in this fashion).
 */
void GR_RUNTIME_API thread_unbind();

/*! \brief Remove any thread-processor affinity for a given thread ID.
 *
 * Note: this does not work on OSX; it is a nop call since OSX
 * does not support the concept of thread affinity (and what they
 * do support in this way since 10.5 is not what we want or can
 * use in this fashion).
 */
void GR_RUNTIME_API thread_unbind(gr_thread_t thread);

/*! \brief get current thread priority for a given thread ID
 */
int GR_RUNTIME_API thread_priority(gr_thread_t thread);

/*! \brief set current thread priority for a given thread ID
 */
int GR_RUNTIME_API set_thread_priority(gr_thread_t thread, int priority);

void GR_RUNTIME_API set_thread_name(gr_thread_t thread, const std::string& name);

} /* namespace thread */
} /* namespace gr */
