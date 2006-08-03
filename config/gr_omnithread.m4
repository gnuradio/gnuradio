# Check for Omnithread (pthread/NT) thread support.             -*- Autoconf -*-

# Copyright 2003 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

AC_DEFUN([GR_OMNITHREAD],
[
  # Check first for POSIX
  ACX_PTHREAD(
  [ ot_posix="yes"
    AC_DEFINE(OMNITHREAD_POSIX,[1],[Define to 1 to enable pthread])
  ],[
    # If no POSIX support found, then check for NT threads
    AC_MSG_CHECKING([for NT threads])

    AC_LINK_IFELSE([
        #include <windows.h>
        #include <winbase.h>
        int main() { InitializeCriticalSection(NULL); return 0; }
      ],
      [ 
        ot_nt="yes"
	AC_DEFINE(OMNITHREAD_NT,[1],[Define to 1 to enable NT thread])
      ],
      [AC_MSG_FAILURE([GNU Radio requires POSIX threads.  pthreads not found.])]
    )
    AC_MSG_RESULT(yes)
  ])
  AM_CONDITIONAL(OMNITHREAD_POSIX, test "x$ot_posix" = xyes)
  AM_CONDITIONAL(OMNITHREAD_NT, test "x$ot_nt" = xyes)

  save_LIBS="$LIBS"
  AC_SEARCH_LIBS([clock_gettime], [rt], [PTHREAD_LIBS="$PTHREAD_LIBS $LIBS"])
  AC_CHECK_FUNCS([clock_gettime gettimeofday nanosleep])
  LIBS="$save_LIBS"
])

