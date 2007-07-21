/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
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
#include "config.h"
#endif

#include <gr_pagesize.h>
#include <unistd.h>
#include <stdio.h>

#if defined(_WIN32) && defined(HAVE_GETPAGESIZE)
extern "C" size_t getpagesize(void);
#endif

int
gr_pagesize ()
{
  static int s_pagesize = -1;

  if (s_pagesize == -1){
#if defined(HAVE_GETPAGESIZE)
    s_pagesize = getpagesize ();
#elif defined (HAVE_SYSCONF)
    s_pagesize = sysconf (_SC_PAGESIZE);
    if (s_pagesize == -1){
      perror ("_SC_PAGESIZE");
      s_pagesize = 4096;
    }
#else
    fprintf (stderr, "gr_pagesize: no info; setting pagesize = 4096\n");
    s_pagesize = 4096;
#endif
  }
  return s_pagesize;
}

