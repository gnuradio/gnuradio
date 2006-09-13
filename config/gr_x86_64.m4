dnl
dnl Copyright 2005 Free Software Foundation, Inc.
dnl 
dnl This file is part of GNU Radio
dnl 
dnl GNU Radio is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2, or (at your option)
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

# GR_X86_64()
#
# Checks to see if we're on a x86_64 machine, and if so, ensure
# that libdir ends in "64"
#
AC_DEFUN([GR_X86_64],[
  AC_REQUIRE([AC_CANONICAL_HOST])
  if test "$host_cpu" = "x86_64"; then
    AC_MSG_CHECKING([libdir for lib64 suffix])
    t=${libdir##*/lib}
    if test "$t" != 64 && test -d /lib64 && ! test -L /lib64; then
      libdir=${libdir}64
      AC_MSG_RESULT([no. Setting libdir to $libdir])
    else
      AC_MSG_RESULT([yes])
    fi
  fi
])
