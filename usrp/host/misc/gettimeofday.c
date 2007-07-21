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

#include <config.h>

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#ifdef HAVE_WINBASE_H
# include <winbase.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

/* 
 * broken implementation for WIN32. 
 * FIXME: usec precision
 */
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	if (tv) {
		time_t tm;
		
		time(&tm);
		tv->tv_sec = tm;
		tv->tv_usec = 0;
	}
	return 0;
}

