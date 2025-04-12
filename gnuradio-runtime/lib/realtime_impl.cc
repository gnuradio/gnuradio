/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2008 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <windows.h>
#endif

#include "realtime_impl.h"
#include <gnuradio/logger.h>
#include <gnuradio/prefs.h>

#ifdef HAVE_SCHED_H
#include <sched.h>
#endif

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>


#if !(defined(_WIN32) || defined(__WIN32__) || defined(WIN32)) && \
    (defined(HAVE_PTHREAD_SETSCHEDPARAM) || defined(HAVE_SCHED_SETSCHEDULER))
#include <pthread.h>

namespace gr {
namespace realtime {

/*!
 * Rescale our virtual priority so that it maps to the middle 1/2 of
 * the priorities given by min_real_pri and max_real_pri.
 */
static int rescale_virtual_pri(int virtual_pri, int min_real_pri, int max_real_pri)
{
    float rmin = min_real_pri + (0.25 * (max_real_pri - min_real_pri));
    float rmax = min_real_pri + (0.75 * (max_real_pri - min_real_pri));
    float m = (rmax - rmin) / (s_rt_priority_max - s_rt_priority_min);
    float y = m * (virtual_pri - s_rt_priority_min) + rmin;
    int y_int = static_cast<int>(rintf(y));
    return std::max(min_real_pri, std::min(max_real_pri, y_int));
}

} // namespace realtime
} // namespace gr

#endif


#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
namespace gr {
namespace realtime {

rt_status_t enable_realtime_scheduling(rt_sched_param p)
{
    // set the priority class on the process
    int pri_class = (true) ? REALTIME_PRIORITY_CLASS : NORMAL_PRIORITY_CLASS;
    if (SetPriorityClass(GetCurrentProcess(), pri_class) == 0)
        return RT_OTHER_ERROR;

    // scale the priority value to the constants
    int priorities[] = { THREAD_PRIORITY_IDLE,         THREAD_PRIORITY_LOWEST,
                         THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL,
                         THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST,
                         THREAD_PRIORITY_TIME_CRITICAL };
    const double priority = double(p.priority) / (s_rt_priority_max - s_rt_priority_min);
    size_t pri_index = size_t((priority + 1.0) * 6 / 2.0); // -1 -> 0, +1 -> 6
    pri_index %= sizeof(priorities) / sizeof(*priorities); // range check

    // set the thread priority on the thread
    if (SetThreadPriority(GetCurrentThread(), priorities[pri_index]) == 0)
        return RT_OTHER_ERROR;
    return RT_OK;
}

} // namespace realtime
} // namespace gr

#elif defined(HAVE_PTHREAD_SETSCHEDPARAM)

namespace gr {
namespace realtime {

rt_status_t enable_realtime_scheduling(rt_sched_param p)
{
    int policy = p.policy == RT_SCHED_FIFO ? SCHED_FIFO : SCHED_RR;
    int min_real_pri = sched_get_priority_min(policy);
    int max_real_pri = sched_get_priority_max(policy);
    int pri = rescale_virtual_pri(p.priority, min_real_pri, max_real_pri);

    // FIXME check hard and soft limits with getrlimit, and limit the value we ask for.
    // log: "pthread_setschedparam: policy = %d, pri = %d\n", policy, pri

    struct sched_param param;
    memset(&param, 0, sizeof(param));
    param.sched_priority = pri;
    int result = pthread_setschedparam(pthread_self(), policy, &param);
    if (result != 0) {
        if (result == EPERM) // N.B., return value, not errno
            return RT_NO_PRIVS;
        else {
            gr::logger_ptr logger, debug_logger;
            gr::configure_default_loggers(logger, debug_logger, "realtime_impl");
            logger->error("pthread_setschedparam: failed to set real time priority: {:s}",
                          strerror(result));
            return RT_OTHER_ERROR;
        }
    }
    return RT_OK;
}

} // namespace realtime
} // namespace gr


#elif defined(HAVE_SCHED_SETSCHEDULER)

namespace gr {
namespace realtime {

rt_status_t enable_realtime_scheduling(rt_sched_param p)
{
    int policy = p.policy == RT_SCHED_FIFO ? SCHED_FIFO : SCHED_RR;
    int min_real_pri = sched_get_priority_min(policy);
    int max_real_pri = sched_get_priority_max(policy);
    int pri = rescale_virtual_pri(p.priority, min_real_pri, max_real_pri);

    // FIXME check hard and soft limits with getrlimit, and limit the value we ask for.
    // log: "sched_setscheduler: policy = %d, pri = %d\n", policy, pri

    int pid = 0; // this process
    struct sched_param param;
    memset(&param, 0, sizeof(param));
    param.sched_priority = pri;
    int result = sched_setscheduler(pid, policy, &param);
    if (result != 0) {
        if (errno == EPERM)
            return RT_NO_PRIVS;
        else {
            gr::logger_ptr logger, debug_logger;
            gr::configure_default_loggers(logger, debug_logger, "realtime_impl");
            logger->error("sched_setscheduler: failed to set real time priority.");
            return RT_OTHER_ERROR;
        }
    }
    return RT_OK;
}

} // namespace realtime
} // namespace gr

#else

namespace gr {
namespace realtime {

rt_status_t enable_realtime_scheduling(rt_sched_param p) { return RT_NOT_IMPLEMENTED; }

} // namespace realtime
} // namespace gr

#endif
