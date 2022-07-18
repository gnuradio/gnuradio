/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/api.h>

////////////////////////////////////////////////////////////////////////
// Use architecture defines to determine the implementation
////////////////////////////////////////////////////////////////////////
#if defined(linux) || defined(__linux) || defined(__linux__)
#define GNURADIO_HRT_USE_CLOCK_GETTIME
#include <ctime>
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define GNURADIO_HRT_USE_QUERY_PERFORMANCE_COUNTER
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#define GNURADIO_HRT_USE_MACH_ABSOLUTE_TIME
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#define GNURADIO_HRT_USE_CLOCK_GETTIME
#include <ctime>
#endif


////////////////////////////////////////////////////////////////////////
namespace gr {

//! Typedef for the timer tick count
using high_res_timer_type = signed long long;

//! Get the current time in ticks
high_res_timer_type high_res_timer_now(void);

//! Get the current time in ticks - for performance monitoring
high_res_timer_type high_res_timer_now_perfmon(void);

//! Get the number of ticks per second
high_res_timer_type high_res_timer_tps(void);

#ifdef GNURADIO_HRT_USE_CLOCK_GETTIME
//! storage for high res timer type
GR_RUNTIME_API extern clockid_t high_res_timer_source;
#endif

} /* namespace gr */

////////////////////////////////////////////////////////////////////////
#ifdef GNURADIO_HRT_USE_CLOCK_GETTIME
inline gr::high_res_timer_type gr::high_res_timer_now(void)
{
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * high_res_timer_tps() + ts.tv_nsec;
}

inline gr::high_res_timer_type gr::high_res_timer_now_perfmon(void)
{
    timespec ts;
    clock_gettime(high_res_timer_source, &ts);
    return ts.tv_sec * high_res_timer_tps() + ts.tv_nsec;
}

inline gr::high_res_timer_type gr::high_res_timer_tps(void) { return 1000000000UL; }
#endif /* GNURADIO_HRT_USE_CLOCK_GETTIME */

////////////////////////////////////////////////////////////////////////
#ifdef GNURADIO_HRT_USE_MACH_ABSOLUTE_TIME
#include <mach/mach_time.h>

inline gr::high_res_timer_type gr::high_res_timer_now(void)
{
    return mach_absolute_time();
}

inline gr::high_res_timer_type gr::high_res_timer_now_perfmon(void)
{
    return gr::high_res_timer_now();
}

inline gr::high_res_timer_type gr::high_res_timer_tps(void)
{
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    return gr::high_res_timer_type(info.numer * 1000000000UL) / info.denom;
}
#endif

////////////////////////////////////////////////////////////////////////
#ifdef GNURADIO_HRT_USE_QUERY_PERFORMANCE_COUNTER
#include <windows.h>

inline gr::high_res_timer_type gr::high_res_timer_now(void)
{
    LARGE_INTEGER counts;
    QueryPerformanceCounter(&counts);
    return counts.QuadPart;
}

inline gr::high_res_timer_type gr::high_res_timer_now_perfmon(void)
{
    return gr::high_res_timer_now();
}

inline gr::high_res_timer_type gr::high_res_timer_tps(void)
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return freq.QuadPart;
}
#endif
