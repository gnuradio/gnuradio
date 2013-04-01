/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <gruel/thread.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)

#include <Windows.h>

namespace gruel {

  gr_thread_t
  get_current_thread_id()
  {
    return GetCurrentThread();
  }

  void
  thread_bind_to_processor(int n)
  {
    std::vector<int> mask(1, n);
    thread_bind_to_processor(get_current_thread_id(), mask);
  }

  void
  thread_bind_to_processor(const std::vector<int> &mask)
  {
    thread_bind_to_processor(get_current_thread_id(), mask);
  }

  void
  thread_bind_to_processor(gr_thread_t thread, int n)
  {
    std::vector<int> mask(1, n);
    thread_bind_to_processor(thread, mask);
  }

  void
  thread_bind_to_processor(gr_thread_t thread, const std::vector<int> &mask)
  {
    //DWORD_PTR mask = (1 << n);
    DWORD_PTR dword_mask = 0;

    std::vector<int> _mask = mask;
    std::vector<int>::iterator itr;
    for(itr = _mask.begin(); itr != _mask.end(); itr++)
      dword_mask |= (1 << (*itr));

    DWORD_PTR ret = SetThreadAffinityMask(thread, dword_mask);
    if(ret == 0) {
      std::stringstream s;
      s << "thread_bind_to_processor failed with error: " << GetLastError() << std::endl;
      throw std::runtime_error(s.str());
    }
  }

  void
  thread_unbind()
  {
    thread_unbind(get_current_thread_id());
  }

  void
  thread_unbind(gr_thread_t thread)
  {
    DWORD_PTR dword_mask = sizeof(DWORD_PTR) - 1;
    DWORD_PTR ret = SetThreadAffinityMask(thread, dword_mask);
    if(ret == 0) {
      std::stringstream s;
      s << "thread_unbind failed with error: " << GetLastError() << std::endl;
      throw std::runtime_error(s.str());
    }
  }
} /* namespace gruel */

#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)

namespace gruel {

  gr_thread_t
  get_current_thread_id()
  {
    // Not implemented on OSX
  }

  void
  thread_bind_to_processor(int n)
  {
    // Not implemented on OSX
  }

  void
  thread_bind_to_processor(gr_thread_t thread, int n)
  {
    // Not implemented on OSX
  }

  void
  thread_bind_to_processor(const std::vector<int> &mask)
  {
    // Not implemented on OSX
  }

  void
  thread_bind_to_processor(gr_thread_t thread, const std::vector<int> &mask)
  {
    // Not implemented on OSX
  }

  void
  thread_unbind()
  {
    // Not implemented on OSX
  }

  void
  thread_unbind(gr_thread_t thread)
  {
    // Not implemented on OSX
  }
} /* namespace gruel */

#else

#include <sstream>
#include <stdexcept>
#include <pthread.h>

namespace gruel {

  gr_thread_t
  get_current_thread_id()
  {
    return pthread_self();
  }

  void
  thread_bind_to_processor(int n)
  {
    std::vector<int> mask(1, n);
    thread_bind_to_processor(get_current_thread_id(), mask);
  }

  void
  thread_bind_to_processor(const std::vector<int> &mask)
  {
    thread_bind_to_processor(get_current_thread_id(), mask);
  }

  void
  thread_bind_to_processor(gr_thread_t thread, int n)
  {
    std::vector<int> mask(1, n);
    thread_bind_to_processor(thread, mask);
  }

  void
  thread_bind_to_processor(gr_thread_t thread, const std::vector<int> &mask)
  {
    cpu_set_t set;
    size_t len = sizeof(cpu_set_t);
    std::vector<int> _mask = mask;
    std::vector<int>::iterator itr;

    CPU_ZERO(&set);
    for(itr = _mask.begin(); itr != _mask.end(); itr++)
      CPU_SET(*itr, &set);

    int ret = pthread_setaffinity_np(thread, len, &set);
    if(ret != 0) {
      std::stringstream s;
      s << "thread_bind_to_processor failed with error: " << ret << std::endl;
      throw std::runtime_error(s.str());
    }
  }

  void
  thread_unbind()
  {
    thread_unbind(get_current_thread_id());
  }

  void
  thread_unbind(gr_thread_t thread)
  {
    cpu_set_t set;
    size_t len = sizeof(cpu_set_t);

    CPU_ZERO(&set);
    long ncpus = sysconf(_SC_NPROCESSORS_ONLN);
    for(long n = 0; n < ncpus; n++) {
      CPU_SET(n, &set);
    }

    int ret = pthread_setaffinity_np(thread, len, &set);
    if(ret != 0) {
      std::stringstream s;
      s << "thread_unbind failed with error: " << ret << std::endl;
      throw std::runtime_error(s.str());
    }
  }
} /* namespace gruel */

#endif
