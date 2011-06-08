/* -*- c++ -*- */
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef HIGH_RES_TIME_FUNCTIONS_H
#define HIGH_RES_TIME_FUNCTIONS_H

#include <ctime>
#include <sys/time.h>
#include <cmath>
/* Requires the librt and libm libraries */

typedef timespec highres_timespec;

static const long NSEC_PER_SEC = 1000000000L;

static inline bool
highres_timespec_greater(const highres_timespec* t1,
			 const highres_timespec* t0)
{
  return ((t1->tv_sec > t0->tv_sec) ||
	  ((t1->tv_sec == t0->tv_sec) &&
	   (t1->tv_nsec > t0->tv_nsec)));
}

static inline bool
highres_timespec_greater(const highres_timespec t1,
			 const highres_timespec t0)
{
  return ((t1.tv_sec > t0.tv_sec) ||
	  ((t1.tv_sec == t0.tv_sec) &&
	   (t1.tv_nsec > t0.tv_nsec)));
}

static inline bool
highres_timespec_less(const highres_timespec* t1,
		      const highres_timespec* t0)
{
  return ((t1->tv_sec < t0->tv_sec) ||
	  ((t1->tv_sec == t0->tv_sec) &&
	   (t1->tv_nsec < t0->tv_nsec)));
}

static inline bool
highres_timespec_less(const highres_timespec t1,
		      const highres_timespec t0)
{
  return ((t1.tv_sec < t0.tv_sec) ||
	  ((t1.tv_sec == t0.tv_sec) &&
	   (t1.tv_nsec < t0.tv_nsec)));
}

static inline bool
highres_timespec_equal(const highres_timespec* t1,
		       const highres_timespec* t0)
{
  return ((t1->tv_sec == t0->tv_sec) &&
	  (t1->tv_nsec == t0->tv_nsec));
}

static inline bool
highres_timespec_equal(const highres_timespec t1,
		       const highres_timespec t0)
{
  return ((t1.tv_sec == t0.tv_sec) &&
	  (t1.tv_nsec == t0.tv_nsec));
}

static inline void
highres_timespec_reset(highres_timespec* ret)
{
  ret->tv_sec = 0;
  ret->tv_nsec = 0;
}

static inline void
set_normalized_highres_timespec(highres_timespec *ts,
				time_t sec, long nsec)
{
  while (nsec > NSEC_PER_SEC) {
    nsec -= NSEC_PER_SEC;
    ++sec;
  }
  while(nsec < 0) {
    nsec += NSEC_PER_SEC;
    --sec;
  }
  ts->tv_sec = sec;
  ts->tv_nsec = nsec;
}

static inline highres_timespec
convert_to_highres_timespec(const double timeValue)
{
  highres_timespec ret;
  double seconds = 0;
  long nsec = static_cast<long>(modf(timeValue, &seconds) * 
				static_cast<double>(NSEC_PER_SEC));
  time_t sec = static_cast<time_t>(seconds);

  set_normalized_highres_timespec(&ret, sec, nsec);

  return ret;
}

static inline double
convert_from_highres_timespec(const highres_timespec actual)
{
  return (static_cast<double>(actual.tv_sec) +
	  (static_cast<double>(actual.tv_nsec) /
	   static_cast<double>(NSEC_PER_SEC)));
}

static inline void
highres_timespec_add(highres_timespec *ret,
		     const highres_timespec* t1,
		     const highres_timespec* t0)
{
  time_t sec = t1->tv_sec + t0->tv_sec;
  long nsec = t1->tv_nsec + t0->tv_nsec;

  set_normalized_highres_timespec(ret, sec, nsec);
}

static inline void
highres_timespec_add(highres_timespec *ret,
		     const highres_timespec t1,
		     const highres_timespec t0)
{
  return highres_timespec_add(ret, &t1, &t0);
}

static inline highres_timespec
highres_timespec_add(const highres_timespec t1,
		     const highres_timespec t0)
{
  highres_timespec ret;
  highres_timespec_add(&ret, &t1, &t0);
  return ret;
}

static inline highres_timespec
highres_timespec_add(const highres_timespec t1,
		     const double time0)
{
  highres_timespec ret;
  highres_timespec t0;
  t0 = convert_to_highres_timespec(time0);

  highres_timespec_add(&ret, &t1, &t0);

  return ret;
}

static inline void
highres_timespec_subtract(highres_timespec *ret,
			  const highres_timespec* t1,
			  const highres_timespec* t0)
{
  time_t sec = t1->tv_sec - t0->tv_sec;
  long nsec = t1->tv_nsec - t0->tv_nsec;

  set_normalized_highres_timespec(ret, sec, nsec);
}

static inline void
highres_timespec_subtract(highres_timespec *ret,
			  const highres_timespec t1,
			  const highres_timespec t0)
{
  return highres_timespec_subtract(ret, &t1, &t0);
}

static inline highres_timespec
highres_timespec_subtract(const highres_timespec t1,
			  const highres_timespec t0)
{
  highres_timespec ret;
  highres_timespec_subtract(&ret, &t1, &t0);
  return ret;
}

static inline highres_timespec
highres_timespec_subtract(const highres_timespec t1,
			  const double time0)
{
  highres_timespec ret;
  highres_timespec t0;
  t0 = convert_to_highres_timespec(time0);

  highres_timespec_subtract(&ret, &t1, &t0);

  return ret;
}

static inline double
diff_highres_timespec(highres_timespec* ret,
		      const highres_timespec *t1,
		      const highres_timespec* t0)
{
  highres_timespec actual;
  time_t sec = 0;
  long nsec = 0;

  if(highres_timespec_greater(t1, t0)){
    sec = t1->tv_sec - t0->tv_sec;
    nsec = t1->tv_nsec - t0->tv_nsec;

    set_normalized_highres_timespec(&actual, sec, nsec);
    
    if(ret != NULL){
      ret->tv_sec = actual.tv_sec;
      ret->tv_nsec = actual.tv_nsec;
    }

    return convert_from_highres_timespec(actual);
  }
  else{
    sec = t0->tv_sec - t1->tv_sec;
    nsec = t0->tv_nsec - t1->tv_nsec;

    // Do nothing with the ret value as the ret value
    // would have to store a negative, which it can't.

    set_normalized_highres_timespec(&actual, sec, nsec);
    
    return (-convert_from_highres_timespec(actual));
  }
}

static inline double
diff_highres_timespec(highres_timespec* ret,
		      const highres_timespec t1,
		      const highres_timespec t0)
{
  return diff_highres_timespec(ret, &t1, &t0);
}

static inline double
diff_highres_timespec(const highres_timespec t1,
		      const highres_timespec t0)
{
  return diff_highres_timespec(NULL, &t1, &t0);
}


static inline double
diff_highres_timespec(const highres_timespec* t1,
		      const highres_timespec* t0)
{
  return diff_highres_timespec(NULL, t1, t0);
}


#ifdef CLOCK_REALTIME
// If we can use clock_gettime, use it;
// otherwise, use gettimeofday
static inline void
get_highres_clock(highres_timespec* ret)
{
  if(clock_gettime(CLOCK_REALTIME, ret) != 0){
    // Unable to get high resolution time - 
    // fail over to low resolution time
    timeval lowResTime;
    gettimeofday(&lowResTime, NULL);
    ret->tv_sec = lowResTime.tv_sec;
    ret->tv_nsec = lowResTime.tv_usec*1000;
  }
}

#else

// Trick timer functions into thinking it has an nsec timer
// but only use the low resolution (usec) timer.
static inline void
get_highres_clock(highres_timespec* ret)
{
  timeval lowResTime;
  gettimeofday(&lowResTime, NULL);
  ret->tv_sec = lowResTime.tv_sec;
  ret->tv_nsec = lowResTime.tv_usec*1000;
}
#endif

static inline highres_timespec
get_highres_clock()
{
  highres_timespec ret;
  get_highres_clock(&ret);
  return ret;
}

static inline bool
highres_timespec_empty(const highres_timespec* ret)
{
  return ( (ret->tv_sec == 0 ) &&  (ret->tv_nsec == 0) );
}

static inline bool
highres_timespec_empty(const highres_timespec ret)
{
  return highres_timespec_empty(&ret);
}

#endif /* HIGH_RES_TIME_FUNCTIONS_H */
