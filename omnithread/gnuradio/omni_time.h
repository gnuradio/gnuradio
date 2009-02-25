/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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
#ifndef INCLUDED_OMNI_TIME_H
#define INCLUDED_OMNI_TIME_H

struct omni_time {
  long int d_secs;	// seconds.
  long int d_nsecs;	// nanoseconds.  Always in [0, 1e9-1]

  omni_time() : d_secs(0), d_nsecs(0) {}
  omni_time(long secs, long nanosecs=0) : d_secs(secs), d_nsecs(nanosecs) {}

  // N.B., this only makes sense for differences between times.
  // Double doesn't have enough bits to precisely represent an absolute time.
  omni_time(double secs);

  // N.B. This only makes sense for differences between times.
  // Double doesn't have enough bits to precisely represent an absolute time.
  double double_time() const { return (double)d_secs + d_nsecs * 1e-9; }

  /*!
   * \brief Return an absolute time suitable for use with
   * schedule_one_shot_timeout & schedule_periodic_timeout
   *
   * The return value is the current time plus the given relative offset.
   */
  static omni_time time(const omni_time &relative_offset = omni_time());
};


inline static bool
operator<(const omni_time &x, const omni_time &y)
{
  return ((x.d_secs < y.d_secs)
	  || (x.d_secs == y.d_secs && x.d_nsecs < y.d_nsecs));
}

inline static bool
operator>(const omni_time &x, const omni_time &y)
{
  return ((x.d_secs > y.d_secs)
	  || (x.d_secs == y.d_secs && x.d_nsecs > y.d_nsecs));
}

inline static bool
operator>=(const omni_time &x, const omni_time &y)
{
  return ((x.d_secs > y.d_secs)
	  || (x.d_secs == y.d_secs && x.d_nsecs >= y.d_nsecs));
}

inline static bool
operator<=(const omni_time &x, const omni_time &y)
{
  return ((x.d_secs < y.d_secs)
	  || (x.d_secs == y.d_secs && x.d_nsecs <= y.d_nsecs));
}

inline static bool
operator==(const omni_time &x, const omni_time &y)
{
  return (x.d_secs == y.d_secs && x.d_nsecs == y.d_nsecs);
}


omni_time operator+(const omni_time &x, const omni_time &y);
omni_time operator+(const omni_time &x, double y);
omni_time operator-(const omni_time &x, const omni_time &y);
omni_time operator-(const omni_time &x, double y);

#endif /* INCLUDED_OMNI_TIME_H */
