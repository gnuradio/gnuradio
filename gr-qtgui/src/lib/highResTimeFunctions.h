#ifndef HIGH_RES_TIME_FUNCTIONS_H
#define HIGH_RES_TIME_FUNCTIONS_H

#include <ctime>
#include <sys/time.h>
#include <cmath>
/* Requires the librt and libm libraries */

static const long NSEC_PER_SEC = 1000000000L;

static inline bool
timespec_greater(const struct timespec* t1,
		 const struct timespec* t0)
{
  return ((t1->tv_sec > t0->tv_sec) ||
	  ((t1->tv_sec == t0->tv_sec) &&
	   (t1->tv_nsec > t0->tv_nsec)));
}

static inline bool
timespec_greater(const struct timespec t1,
		 const struct timespec t0)
{
  return ((t1.tv_sec > t0.tv_sec) ||
	  ((t1.tv_sec == t0.tv_sec) &&
	   (t1.tv_nsec > t0.tv_nsec)));
}

static inline bool
timespec_less(const struct timespec* t1,
	      const struct timespec* t0)
{
  return ((t1->tv_sec < t0->tv_sec) ||
	  ((t1->tv_sec == t0->tv_sec) &&
	   (t1->tv_nsec < t0->tv_nsec)));
}

static inline bool
timespec_less(const struct timespec t1,
	      const struct timespec t0)
{
  return ((t1.tv_sec < t0.tv_sec) ||
	  ((t1.tv_sec == t0.tv_sec) &&
	   (t1.tv_nsec < t0.tv_nsec)));
}

static inline bool
timespec_equal(const struct timespec* t1,
	       const struct timespec* t0)
{
  return ((t1->tv_sec == t0->tv_sec) &&
	  (t1->tv_nsec == t0->tv_nsec));
}

static inline bool
timespec_equal(const struct timespec t1,
	       const struct timespec t0)
{
  return ((t1.tv_sec == t0.tv_sec) &&
	  (t1.tv_nsec == t0.tv_nsec));
}

static inline void
timespec_reset(struct timespec* ret)
{
  ret->tv_sec = 0;
  ret->tv_nsec = 0;
}

static inline void
set_normalized_timespec(struct timespec *ts,
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

static inline struct timespec
convert_to_timespec(const double timeValue)
{
  struct timespec ret;
  double seconds = 0;
  long nsec = static_cast<long>(modf(timeValue, &seconds) * 
				static_cast<double>(NSEC_PER_SEC));
  time_t sec = static_cast<time_t>(seconds);

  set_normalized_timespec(&ret, sec, nsec);

  return ret;
}

static inline double
convert_from_timespec(const timespec actual)
{
  return (static_cast<double>(actual.tv_sec) +
	  (static_cast<double>(actual.tv_nsec) /
	   static_cast<double>(NSEC_PER_SEC)));
}

static inline void
timespec_add(struct timespec *ret,
	     const struct timespec* t1,
	     const struct timespec* t0)
{
  time_t sec = t1->tv_sec + t0->tv_sec;
  long nsec = t1->tv_nsec + t0->tv_nsec;

  set_normalized_timespec(ret, sec, nsec);
}

static inline void
timespec_add(struct timespec *ret,
	     const struct timespec t1,
	     const struct timespec t0)
{
  return timespec_add(ret, &t1, &t0);
}

static inline struct timespec
timespec_add(const struct timespec t1,
	     const struct timespec t0)
{
  struct timespec ret;
  timespec_add(&ret, &t1, &t0);
  return ret;
}

static inline struct timespec
timespec_add(const struct timespec t1,
	     const double time0)
{
  struct timespec ret;
  struct timespec t0;
  t0 = convert_to_timespec(time0);

  timespec_add(&ret, &t1, &t0);

  return ret;
}

static inline void
timespec_subtract(struct timespec *ret,
		  const struct timespec* t1,
		  const struct timespec* t0)
{
  time_t sec = t1->tv_sec - t0->tv_sec;
  long nsec = t1->tv_nsec - t0->tv_nsec;

  set_normalized_timespec(ret, sec, nsec);
}

static inline void
timespec_subtract(struct timespec *ret,
		  const struct timespec t1,
		  const struct timespec t0)
{
  return timespec_subtract(ret, &t1, &t0);
}

static inline struct timespec
timespec_subtract(const struct timespec t1,
		  const struct timespec t0)
{
  struct timespec ret;
  timespec_subtract(&ret, &t1, &t0);
  return ret;
}

static inline struct timespec
timespec_subtract(const struct timespec t1,
		  const double time0)
{
  struct timespec ret;
  struct timespec t0;
  t0 = convert_to_timespec(time0);

  timespec_subtract(&ret, &t1, &t0);

  return ret;
}

static inline double
diff_timespec(struct timespec* ret,
	      const struct timespec *t1,
	      const struct timespec* t0)
{
  struct timespec actual;
  time_t sec = 0;
  long nsec = 0;

  if(timespec_greater(t1, t0)){
    sec = t1->tv_sec - t0->tv_sec;
    nsec = t1->tv_nsec - t0->tv_nsec;

    set_normalized_timespec(&actual, sec, nsec);
    
    if(ret != NULL){
      ret->tv_sec = actual.tv_sec;
      ret->tv_nsec = actual.tv_nsec;
    }

    return convert_from_timespec(actual);
  }
  else{
    sec = t0->tv_sec - t1->tv_sec;
    nsec = t0->tv_nsec - t1->tv_nsec;

    // Do nothing with the ret value as the ret value
    // would have to store a negative, which it can't.

    set_normalized_timespec(&actual, sec, nsec);
    
    return (-convert_from_timespec(actual));
  }
}

static inline double
diff_timespec(struct timespec* ret,
	      const struct timespec t1,
	      const struct timespec t0)
{
  return diff_timespec(ret, &t1, &t0);
}

static inline double
diff_timespec(const struct timespec t1,
	      const struct timespec t0)
{
  return diff_timespec(NULL, &t1, &t0);
}


static inline double
diff_timespec(const struct timespec* t1,
	      const struct timespec* t0)
{
  return diff_timespec(NULL, t1, t0);
}


#ifdef CLOCK_REALTIME
// If we can use clock_gettime, use it;
// otherwise, use gettimeofday
static inline void
get_highres_clock(struct timespec* ret)
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
get_highres_clock(struct timespec* ret)
{
  timeval lowResTime;
  gettimeofday(&lowResTime, NULL);
  ret->tv_sec = lowResTime.tv_sec;
  ret->tv_nsec = lowResTime.tv_usec*1000;
}
#endif

static inline struct timespec
get_highres_clock()
{
  struct timespec ret;
  get_highres_clock(&ret);
  return ret;
}

static inline bool
timespec_empty(const struct timespec* ret)
{
  return ( (ret->tv_sec == 0 ) &&  (ret->tv_nsec == 0) );
}

static inline bool
timespec_empty(const struct timespec ret)
{
  return timespec_empty(&ret);
}

#endif /* HIGH_RES_TIME_FUNCTIONS_H */
