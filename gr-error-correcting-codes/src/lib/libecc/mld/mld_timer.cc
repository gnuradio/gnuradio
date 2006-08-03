/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio.
 *
 * Primary Author: Michael Dickens, NCIP Lab, University of Notre Dame
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <sys/types.h>
#include <mld_timer.h>

void start_timer (struct timeval *t_tp)
{
  gettimeofday (t_tp, 0);
}

u_long end_timer (struct timeval *g_tp)
{
  struct timeval t_tp;
  gettimeofday (&t_tp, 0);

  u_long retVal = (t_tp.tv_sec - g_tp->tv_sec);
  u_long df_usec;

  if (t_tp.tv_usec < g_tp->tv_usec) {
    retVal -= 1;
    df_usec = 1000000 - (g_tp->tv_usec - t_tp.tv_usec);
  } else
    df_usec = t_tp.tv_usec - g_tp->tv_usec;

  retVal *= 1000000;
  retVal += df_usec;
  return (retVal);
}
