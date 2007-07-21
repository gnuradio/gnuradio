/* Copyright (C) 1992 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 3 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

#include <config.h>

#ifndef HAVE_USLEEP

#include <sys/types.h>
#include <sys/time.h>

#ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#ifdef HAVE_WINBASE_H
# include <winbase.h>
#endif

#ifdef apollo
# include <apollo/base.h>
# include <apollo/time.h>
  static time_$clock_t DomainTime100mS =
    {
	0, 100000/4
    };
  static status_$t DomainStatus;
#endif

/* Sleep USECONDS microseconds, or until a previously set timer goes off.  */
int
usleep (unsigned long useconds)
{
#ifdef apollo
  /* The usleep function does not work under the SYS5.3 environment.
     Use the Domain/OS time_$wait call instead. */
  time_$wait (time_$relative, DomainTime100mS, &DomainStatus);
#elif defined(HAVE_SSLEEP)	/* Win32 */
  Sleep( useconds/1000 );
#else
  struct timeval delay;

  delay.tv_sec = 0;
  delay.tv_usec = useconds;
  select (0, 0, 0, 0, &delay);
#endif
  return 0;
}

#endif /* !HAVE_USLEEP */
