/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GNURADIO_HIGH_RES_TIMER_H
#define INCLUDED_GNURADIO_HIGH_RES_TIMER_H

////////////////////////////////////////////////////////////////////////
// Use architecture defines to determine the implementation
////////////////////////////////////////////////////////////////////////
#if defined(linux) || defined(__linux) || defined(__linux__)
#define GNURADIO_HRT_USE_CLOCK_GETTIME
#include <ctime>
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <windows.h>
#define GNURADIO_HRT_USE_QUERY_PERFORMANCE_COUNTER
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#include <mach/mach_time.h>
#define GNURADIO_HRT_USE_MACH_ABSOLUTE_TIME
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#define GNURADIO_HRT_USE_CLOCK_GETTIME
#include <ctime>
#else
#include <ratio>
#define GNURADIO_HRT_USE_GENERIC_CLOCK
#endif

#include <gnuradio/api.h>
#include <chrono>

////////////////////////////////////////////////////////////////////////
namespace gr {

//! Typedef for the timer tick count
typedef signed long long high_res_timer_type;

//! Get the current time in ticks
high_res_timer_type high_res_timer_now(void);

//! Get the current time in ticks - for performance monitoring
high_res_timer_type high_res_timer_now_perfmon(void);

//! Get the number of ticks per second
high_res_timer_type high_res_timer_tps(void);

//! Get the tick count at the epoch
high_res_timer_type high_res_timer_epoch(void);

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
    return gr::high_res_timer_type(info.denom * 1000000000UL) / info.numer;
}
#endif

////////////////////////////////////////////////////////////////////////
#ifdef GNURADIO_HRT_USE_QUERY_PERFORMANCE_COUNTER

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

////////////////////////////////////////////////////////////////////////
#ifdef GNURADIO_HRT_USE_GENERIC_CLOCK
inline gr::high_res_timer_type gr::high_res_timer_now(void)
{
    return std::chrono::duration<gr::high_res_timer_type, std::nano>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

inline gr::high_res_timer_type gr::high_res_timer_now_perfmon(void)
{
    return gr::high_res_timer_now();
}

inline gr::high_res_timer_type gr::high_res_timer_tps(void) { return 1000000000UL; }
#endif

////////////////////////////////////////////////////////////////////////
inline gr::high_res_timer_type gr::high_res_timer_epoch(void)
{
    static const double ticks_per_second = gr::high_res_timer_tps();
    const double seconds_since_epoch =
        std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    return gr::high_res_timer_now() - seconds_since_epoch * ticks_per_second;
}

#endif /* INCLUDED_GNURADIO_HIGH_RES_TIMER_H */
