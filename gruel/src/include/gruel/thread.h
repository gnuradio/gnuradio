/* -*- c++ -*- */
/*
 * Copyright 2009-2012 Free Software Foundation, Inc.
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
#ifndef INCLUDED_THREAD_H
#define INCLUDED_THREAD_H

#include <gruel/api.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition_variable.hpp>
#include <vector>

namespace gruel {

  typedef boost::thread                    thread;
  typedef boost::mutex                     mutex;
  typedef boost::mutex::scoped_lock        scoped_lock;
  typedef boost::condition_variable        condition_variable;

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
  typedef HANDLE gr_thread_t;
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
  typedef pthread_t gr_thread_t;
#else
  typedef pthread_t gr_thread_t;
#endif

  GRUEL_API gr_thread_t get_current_thread_id();
  GRUEL_API void thread_bind_to_processor(const std::vector<unsigned int> &mask);
  GRUEL_API void thread_bind_to_processor(unsigned int n);
  GRUEL_API void thread_bind_to_processor(gr_thread_t thread, const std::vector<unsigned int> &mask);
  GRUEL_API void thread_bind_to_processor(gr_thread_t thread, unsigned int n);
  GRUEL_API void thread_unbind();
  GRUEL_API void thread_unbind(gr_thread_t thread);

} /* namespace gruel */

#endif /* INCLUDED_THREAD_H */
