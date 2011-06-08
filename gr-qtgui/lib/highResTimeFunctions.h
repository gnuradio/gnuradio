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

#include <cmath>
#include <gruel/high_res_timer.h>

typedef gruel::high_res_timer_type highres_timespec;

static inline void
highres_timespec_reset(highres_timespec &ret)
{
  ret = 0;
}

static inline highres_timespec
highres_timespec_add(const highres_timespec &t1, double t)
{
  return t1 + t*gruel::high_res_timer_tps();
}

static inline double
diff_highres_timespec(const highres_timespec &t1,
		      const highres_timespec &t0)
{
  return std::abs(double(t1 - t0)/gruel::high_res_timer_tps());
}

static inline highres_timespec
get_highres_clock()
{
  return gruel::high_res_timer_now();
}

static inline bool
highres_timespec_empty(const highres_timespec &t)
{
    return t == 0;
}


#endif /* HIGH_RES_TIME_FUNCTIONS_H */
