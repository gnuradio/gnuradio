/* -*- c++ -*- */
/*
 * Copyright 2012-2014 Free Software Foundation, Inc.
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

#include <gnuradio/thread/thread.h>
#include <boost/format.hpp>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)

#include <windows.h>

namespace gr {
  namespace thread {

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
        s << "thread_bind_to_processor failed with error: "
          << GetLastError() << std::endl;
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
        s << "thread_unbind failed with error: "
          << GetLastError() << std::endl;
        throw std::runtime_error(s.str());
      }
    }

    int
    thread_priority(gr_thread_t thread)
    {
      // Not implemented on Windows
      return -1;
    }

    int
    set_thread_priority(gr_thread_t thread, int priority)
    {
      // Not implemented on Windows
      return -1;
    }
#ifndef __MINGW32__
#pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO
    {
      DWORD dwType;     // Must be 0x1000
      LPCSTR szName;    // Pointer to name (in user addr space)
      DWORD dwThreadID; // Thread ID (-1 = caller thread)
      DWORD dwFlags;    // Reserved for future use, must be zero
    } THREADNAME_INFO;
#pragma pack(pop)
    static void
    _set_thread_name(gr_thread_t thread, const char* name, DWORD dwThreadId)
    {
      const DWORD SET_THREAD_NAME_EXCEPTION = 0x406D1388;

      THREADNAME_INFO info;
      info.dwType = 0x1000;
      info.szName = name;
      info.dwThreadID = dwThreadId;
      info.dwFlags = 0;

      __try
      {
        RaiseException(SET_THREAD_NAME_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info);
      }
      __except(EXCEPTION_EXECUTE_HANDLER)
      {
      }
    }

    void
    set_thread_name(gr_thread_t thread, std::string name)
    {
      DWORD dwThreadId = GetThreadId(thread);
      if (dwThreadId == 0)
        return;

      if (name.empty())
        name = boost::str(boost::format("thread %lu") % dwThreadId);

      _set_thread_name(thread, name.c_str(), dwThreadId);
    }
#else
    void
    set_thread_name(gr_thread_t thread, std::string name)
    {
        /* Not implemented on mingw-w64 */
    }
#endif /* !__MINGW32__ */

  } /* namespace thread */
} /* namespace gr */


#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__) || \
  defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__GNU__)

namespace gr {
  namespace thread {

    gr_thread_t
    get_current_thread_id()
    {
      // Not implemented on OSX
      return NULL;
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

    int
    thread_priority(gr_thread_t thread)
    {
      sched_param param;
      int priority;
      int policy;
      int ret;
      ret = pthread_getschedparam (thread, &policy, &param);
      priority = param.sched_priority;
      return (ret==0)?priority:ret;
    }

    int
    set_thread_priority(gr_thread_t thread, int priority)
    {
      int policy;
      struct sched_param param;
      pthread_getschedparam (thread, &policy, &param);
      param.sched_priority = priority;
      return pthread_setschedparam(thread, policy, &param);
    }

    void
    set_thread_name(gr_thread_t thread, std::string name)
    {
      // Not implemented on OSX
    }

  } /* namespace thread */
} /* namespace gr */

#else

#include <sstream>
#include <stdexcept>
#include <pthread.h>
#include <sys/prctl.h>

namespace gr {
  namespace thread {

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

    int
    thread_priority(gr_thread_t thread)
    {
      sched_param param;
      int priority;
      int policy;
      int ret;
      ret = pthread_getschedparam (thread, &policy, &param);
      priority = param.sched_priority;
      return (ret==0)?priority:ret;
    }

    int
    set_thread_priority(gr_thread_t thread, int priority)
    {
      int policy;
      struct sched_param param;
      pthread_getschedparam (thread, &policy, &param);
      param.sched_priority = priority;
      return pthread_setschedparam(thread, policy, &param);
    }

    void
    set_thread_name(gr_thread_t thread, std::string name)
    {
      if (thread != pthread_self()) // Naming another thread is not supported
        return;

      if (name.empty())
        name = boost::str(boost::format("thread %llu") % ((unsigned long long)thread));

      const int max_len = 16; // Maximum accepted by PR_SET_NAME

      if ((int)name.size() > max_len) // Shorten the name if necessary by taking as many characters from the front
      {                               // so that the unique_id can still fit on the end
        int i = name.size() - 1;
        for (; i >= 0; --i)
        {
          std::string s = name.substr(i, 1);
          int n = atoi(s.c_str());
          if ((n == 0) && (s != "0"))
            break;
        }

        name = name.substr(0, std::max(0, max_len - ((int)name.size() - (i + 1)))) + name.substr(i + 1);
      }

      prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
    }

  } /* namespace thread */
} /* namespace gr */

#endif
