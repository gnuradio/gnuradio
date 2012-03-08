dnl
dnl Copyright 2002 Free Software Foundation, Inc.
dnl 
dnl This file is part of GNU Radio
dnl 
dnl GNU Radio is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 3, or (at your option)
dnl any later version.
dnl 
dnl GNU Radio is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with GNU Radio; see the file COPYING.  If not, write to
dnl the Free Software Foundation, Inc., 51 Franklin Street,
dnl Boston, MA 02110-1301, USA.
dnl 

dnl FIXME probably need to add linker flag too...

AC_DEFUN([GR_SET_GPROF],[
  dnl Check for --with-gprof
  AC_MSG_CHECKING([whether user wants gprof])
  AC_ARG_WITH(gprof,
              [  --with-gprof            Turn on gprof profiling],
              [], [ with_gprof=no ])
  AC_MSG_RESULT($with_gprof)
  
  dnl gprof profiling flags for the two main compilers
  cc_profiling_flags="-pg"
  cxx_profiling_flags="-pg"
  ld_profiling_flags="-pg"
  if test $with_gprof = yes
  then
    if test -n "${CC}"
    then
      LF_CHECK_CC_FLAG($cc_profiling_flags)
    fi
    if test -n "${CXX}" 
    then
      LF_CHECK_CXX_FLAG($cxx_profiling_flags)
    fi
  fi
])

AC_DEFUN([GR_SET_PROF],[
  dnl Check for --with-prof
  AC_MSG_CHECKING([whether user wants prof])
  AC_ARG_WITH(prof,
              [  --with-prof             Turn on prof profiling],
              [], [ with_prof=no ])
  AC_MSG_RESULT($with_prof)
  
  dnl prof profiling flags for the two main compilers
  cc_profiling_flags="-p"
  cxx_profiling_flags="-p"
  ld_profiling_flags="-p"
  if test $with_prof = yes
  then
    if test -n "${CC}"
    then
      LF_CHECK_CC_FLAG($cc_profiling_flags)
    fi
    if test -n "${CXX}" 
    then
      LF_CHECK_CXX_FLAG($cxx_profiling_flags)
    fi
  fi
])
