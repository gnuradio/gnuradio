# Check for (mingw)win32 POSIX replacements.             -*- Autoconf -*-

# Copyright 2003,2004,2005 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.


AC_DEFUN([GR_PWIN32],
[
AC_REQUIRE([AC_HEADER_TIME])
AC_CHECK_HEADERS([sys/types.h fcntl.h io.h])
AC_CHECK_HEADERS([windows.h])
AC_CHECK_HEADERS([winioctl.h winbase.h], [], [], [
	#if HAVE_WINDOWS_H
	#include <windows.h>
	#endif
])

AC_CHECK_FUNCS([getopt usleep gettimeofday nanosleep rand srand random srandom sleep sigaction])
AC_CHECK_TYPES([struct timezone, struct timespec, ssize_t],[],[],[
     #if HAVE_SYS_TYPES_H
     # include <sys/types.h>
     #endif
     #if TIME_WITH_SYS_TIME
     # include <sys/time.h>
     # include <time.h>
     #else
     # if HAVE_SYS_TIME_H
     #  include <sys/time.h>
     # else
     #  include <time.h>
     # endif
     #endif
])

dnl Checks for replacements
AC_REPLACE_FUNCS([getopt usleep gettimeofday])


AC_MSG_CHECKING(for Sleep)
AC_TRY_LINK([   #include <windows.h>
                #include <winbase.h>
                ], [ Sleep(0); ],
                [AC_DEFINE(HAVE_SSLEEP,1,[Define to 1 if you have win32 Sleep])
                AC_MSG_RESULT(yes)],
                AC_MSG_RESULT(no)
                )

dnl Under Win32, mkdir prototype in io.h has only one arg
AC_MSG_CHECKING(whether mkdir accepts only one arg)
AC_TRY_COMPILE([#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>], [
       mkdir("")
 ], [ AC_MSG_RESULT(yes)
     AC_DEFINE(MKDIR_TAKES_ONE_ARG,[],[Define if mkdir accepts only one arg]) ],
 [ AC_MSG_RESULT(no)
 ])

AH_BOTTOM(
[
/* Define missing prototypes, implemented in replacement lib */
#ifdef  __cplusplus
extern "C" {
#endif

#ifndef HAVE_GETOPT
int getopt (int argc, char * const argv[], const char * optstring);
extern char * optarg;
extern int optind, opterr, optopt;
#endif

#ifndef HAVE_USLEEP
int usleep(unsigned long usec);	/* SUSv2 */
#endif

#ifndef HAVE_NANOSLEEP
#ifndef HAVE_STRUCT_TIMESPEC
#if HAVE_SYS_TYPES_H
# include <sys/types.h>	/* need time_t */
#endif
struct timespec {
	time_t	tv_sec;
	long	tv_nsec;
};
#endif
static inline int nanosleep(const struct timespec *req, struct timespec *rem) { return usleep(req->tv_sec*1000000+req->tv_nsec/1000); }
#endif

#if defined(HAVE_SSLEEP) && !defined(HAVE_SLEEP)
#ifdef HAVE_WINBASE_H
#include <windows.h>
#include <winbase.h>
#endif
/* TODO: what about SleepEx? */
static inline unsigned int sleep (unsigned int nb_sec) { Sleep(nb_sec*1000); return 0; }
#endif

#ifndef HAVE_GETTIMEOFDAY
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifndef HAVE_STRUCT_TIMEZONE
struct timezone {
        int  tz_minuteswest;
	int  tz_dsttime;
};
#endif
int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

#if !defined(HAVE_RANDOM) && defined(HAVE_RAND)
#include <stdlib.h>
static inline long int random (void) { return rand(); }
#endif

#if !defined(HAVE_SRANDOM) && defined(HAVE_SRAND)
static inline void srandom (unsigned int seed) { srand(seed); }
#endif

#ifndef HAVE_SSIZE_T
typedef size_t ssize_t;
#endif

#ifdef  __cplusplus
}
#endif
])


])
