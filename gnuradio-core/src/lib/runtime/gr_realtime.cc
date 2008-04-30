/* -*- c++ -*- */
/*
 * Copyright 2006,2007 Free Software Foundation, Inc.
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
#include <gr_realtime.h>

#ifdef HAVE_SCHED_H
#include <sched.h>
#endif

#include <string.h>
#include <errno.h>
#include <stdio.h>

#if defined(HAVE_SCHED_SETSCHEDULER)

gr_rt_status_t
gr_enable_realtime_scheduling()
{
  int policy = SCHED_FIFO;
  int pri = (sched_get_priority_max (policy) + sched_get_priority_min (policy)) / 2;
  int pid = 0;  // this process

  struct sched_param param;
  memset(&param, 0, sizeof(param));
  param.sched_priority = pri;
  int result = sched_setscheduler(pid, policy, &param);
  if (result != 0){
    if (errno == EPERM)
      return RT_NO_PRIVS;
    else {
      perror ("sched_setscheduler: failed to set real time priority");
      return RT_OTHER_ERROR;
    }
  }
  //printf("SCHED_FIFO enabled with priority = %d\n", pri);
  return RT_OK;
}

#elif defined(HAVE_PTHREAD_SETSCHEDPARAM)

#include <pthread.h>
#include <stdio.h>

gr_rt_status_t
gr_enable_realtime_scheduling()
{
  int policy = SCHED_FIFO;
  int pri = (sched_get_priority_max (policy) +
	     sched_get_priority_min (policy)) / 2;
  pthread_t this_thread = pthread_self ();  // this process
  struct sched_param param;
  memset (&param, 0, sizeof (param));
  param.sched_priority = pri;
  int result = pthread_setschedparam (this_thread, policy, &param);
  if (result != 0) {
    if (errno == EPERM)
      return RT_NO_PRIVS;
    else {
      perror ("pthread_setschedparam: failed to set real time priority");
      return RT_OTHER_ERROR;
    }
  }
  //printf("SCHED_FIFO enabled with priority = %d\n", pri);
  return RT_OK;
}

// #elif // could try negative niceness

#else

gr_rt_status_t
gr_enable_realtime_scheduling()
{
  return RT_NOT_IMPLEMENTED;
}

#endif
