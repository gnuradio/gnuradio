/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GRUEL_HIGH_RES_TIMER_H
#define INCLUDED_GRUEL_HIGH_RES_TIMER_H

namespace gruel {
    //! Typedef for the timer tick count
    typedef signed long long high_res_timer_type;

    //! Get the current time in ticks
    high_res_timer_type high_res_timer_now(void);

    //! Get the number of ticks per second
    high_res_timer_type high_res_timer_tps(void);

    //! Get the tick count at the epoch
    high_res_timer_type high_res_timer_epoch(void);

} /* namespace gruel */

////////////////////////////////////////////////////////////////////////
// Use architecture defines to determine the implementation
////////////////////////////////////////////////////////////////////////
#if defined(linux) || defined(__linux) || defined(__linux__)
    #define GRUEL_HRT_USE_CLOCK_GETTIME
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #define GRUEL_HRT_USE_QUERY_PERFORMANCE_COUNTER
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    #define GRUEL_HRT_USE_MACH_ABSOLUTE_TIME
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    #define GRUEL_HRT_USE_CLOCK_GETTIME
#else
    #define GRUEL_HRT_USE_MICROSEC_CLOCK
#endif

////////////////////////////////////////////////////////////////////////
#ifdef GRUEL_HRT_USE_CLOCK_GETTIME
    #include <ctime>

    inline gruel::high_res_timer_type gruel::high_res_timer_now(void){
        timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec*high_res_timer_tps() + ts.tv_nsec;
    }

    inline gruel::high_res_timer_type gruel::high_res_timer_tps(void){
        return 1000000000UL;
    }
#endif /* GRUEL_HRT_USE_CLOCK_GETTIME */

////////////////////////////////////////////////////////////////////////
#ifdef GRUEL_HRT_USE_MACH_ABSOLUTE_TIME
    #include <mach/mach_time.h>

    inline gruel::high_res_timer_type gruel::high_res_timer_now(void){
        return mach_absolute_time();
    }

    inline gruel::high_res_timer_type gruel::high_res_timer_tps(void){
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);
        return gruel::high_res_timer_type(info.numer*1000000000UL)/info.denom;
    }
#endif

////////////////////////////////////////////////////////////////////////
#ifdef GRUEL_HRT_USE_QUERY_PERFORMANCE_COUNTER
    #include <Windows.h>

    inline gruel::high_res_timer_type gruel::high_res_timer_now(void){
        LARGE_INTEGER counts;
        QueryPerformanceCounter(&counts);
        return counts.QuadPart;
    }

    inline gruel::high_res_timer_type gruel::high_res_timer_tps(void){
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return freq.QuadPart;
    }
#endif

////////////////////////////////////////////////////////////////////////
#ifdef GRUEL_HRT_USE_MICROSEC_CLOCK
    #include <boost/date_time/posix_time/posix_time.hpp>

    inline gruel::high_res_timer_type gruel::high_res_timer_now(void){
        static const boost::posix_time::ptime epoch(boost::posix_time::from_time_t(0));
        return (boost::posix_time::microsec_clock::universal_time() - epoch).ticks();
    }

    inline gruel::high_res_timer_type gruel::high_res_timer_tps(void){
        return boost::posix_time::time_duration::ticks_per_second();
    }
#endif

////////////////////////////////////////////////////////////////////////
#include <boost/date_time/posix_time/posix_time.hpp>

inline gruel::high_res_timer_type gruel::high_res_timer_epoch(void){
    static const double hrt_ticks_per_utc_ticks = gruel::high_res_timer_tps()/double(boost::posix_time::time_duration::ticks_per_second());
    boost::posix_time::time_duration utc = boost::posix_time::microsec_clock::universal_time() - boost::posix_time::from_time_t(0);
    return gruel::high_res_timer_now() - utc.ticks()*hrt_ticks_per_utc_ticks;
}

#endif /* INCLUDED_GRUEL_HIGH_RES_TIMER_H */
