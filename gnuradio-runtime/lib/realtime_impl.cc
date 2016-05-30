/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/realtime_impl.h>

#ifdef HAVE_SCHED_H
#include <sched.h>
#endif

#include <algorithm>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include <iostream>

#if defined(HAVE_PTHREAD_SETSCHEDPARAM) || defined(HAVE_SCHED_SETSCHEDULER)
#include <pthread.h>

namespace gr {
  namespace impl {

    /*!
     * Rescale our virtual priority so that it maps to the middle 1/2 of
     * the priorities given by min_real_pri and max_real_pri.
     */
    static int
    rescale_virtual_pri(int virtual_pri, int min_real_pri, int max_real_pri)
    {
      float rmin = min_real_pri + (0.25 * (max_real_pri - min_real_pri));
      float rmax = min_real_pri + (0.75 * (max_real_pri - min_real_pri));
      float m = (rmax - rmin) / (rt_priority_max() - rt_priority_min());
      float y = m * (virtual_pri - rt_priority_min()) + rmin;
      int   y_int = static_cast<int>(rint(y));
      return std::max(min_real_pri, std::min(max_real_pri, y_int));
    }

  } // namespace impl
}  // namespace gr

#endif


#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)

#include <windows.h>

namespace gr {
  namespace impl {

    rt_status_t enable_realtime_scheduling(rt_sched_param p)
    {
      //set the priority class on the process
      int pri_class = (true)? REALTIME_PRIORITY_CLASS : NORMAL_PRIORITY_CLASS;
      if(SetPriorityClass(GetCurrentProcess(), pri_class) == 0)
        return RT_OTHER_ERROR;

      //scale the priority value to the constants
      int priorities[] = {
        THREAD_PRIORITY_IDLE, THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL,
        THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_TIME_CRITICAL
      };
      const double priority = double(p.priority)/(rt_priority_max() - rt_priority_min());
      size_t pri_index = size_t((priority+1.0)*6/2.0); // -1 -> 0, +1 -> 6
      pri_index %= sizeof(priorities)/sizeof(*priorities); //range check

      //set the thread priority on the thread
      if(SetThreadPriority(GetCurrentThread(), priorities[pri_index]) == 0)
        return RT_OTHER_ERROR;

      //printf("SetPriorityClass + SetThreadPriority\n");
      return RT_OK;
    }

  } // namespace impl
} // namespace gr

#elif defined(HAVE_PTHREAD_SETSCHEDPARAM)

namespace gr {
  namespace impl {

    rt_status_t
    enable_realtime_scheduling(rt_sched_param p)
    {
      int policy = p.policy == RT_SCHED_FIFO ? SCHED_FIFO : SCHED_RR;
      int min_real_pri = sched_get_priority_min(policy);
      int max_real_pri = sched_get_priority_max(policy);
      int pri = rescale_virtual_pri(p.priority, min_real_pri, max_real_pri);

      // FIXME check hard and soft limits with getrlimit, and limit the value we ask for.
      // fprintf(stderr, "pthread_setschedparam: policy = %d, pri = %d\n", policy, pri);

      struct sched_param param;
      memset (&param, 0, sizeof (param));
      param.sched_priority = pri;
      int result = pthread_setschedparam (pthread_self(), policy, &param);
      if(result != 0) {
        if(result == EPERM)	// N.B., return value, not errno
          return RT_NO_PRIVS;
        else {
          fprintf(stderr,
                  "pthread_setschedparam: failed to set real time priority: %s\n",
                  strerror(result));
          return RT_OTHER_ERROR;
        }
      }

      //printf("SCHED_FIFO enabled with priority = %d\n", pri);
      return RT_OK;
    }

  } // namespace impl
} // namespace gr


#elif defined(HAVE_SCHED_SETSCHEDULER)

namespace gr {
  namespace impl {

    rt_status_t
    enable_realtime_scheduling(rt_sched_param p)
    {
      int policy = p.policy == RT_SCHED_FIFO ? SCHED_FIFO : SCHED_RR;
      int min_real_pri = sched_get_priority_min(policy);
      int max_real_pri = sched_get_priority_max(policy);
      int pri = rescale_virtual_pri(p.priority, min_real_pri, max_real_pri);

      // FIXME check hard and soft limits with getrlimit, and limit the value we ask for.
      // fprintf(stderr, "sched_setscheduler: policy = %d, pri = %d\n", policy, pri);

      int pid = 0;  // this process
      struct sched_param param;
      memset(&param, 0, sizeof(param));
      param.sched_priority = pri;
      int result = sched_setscheduler(pid, policy, &param);
      if(result != 0){
        if(errno == EPERM)
          return RT_NO_PRIVS;
        else {
          perror("sched_setscheduler: failed to set real time priority");
          return RT_OTHER_ERROR;
        }
      }

      //printf("SCHED_FIFO enabled with priority = %d\n", pri);
      return RT_OK;
    }

  } // namespace impl
} // namespace gr

#else

namespace gr {
  namespace impl {

    rt_status_t
    enable_realtime_scheduling(rt_sched_param p)
    {
      return RT_NOT_IMPLEMENTED;
    }

  } // namespace impl
} // namespace gr

#endif
