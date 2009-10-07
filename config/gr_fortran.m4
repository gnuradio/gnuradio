dnl Copyright 2001,2002,2003,2004,2005,2006 Free Software Foundation, Inc.
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

AC_DEFUN([GR_FORTRAN],[
    dnl if you want to generate a different table of interpolator taps, you need fortran.
    dnl we default to off, since almost no one wants to do this.
    AC_ARG_ENABLE(fortran, AC_HELP_STRING([--enable-fortran],[enable fortran (no)]),
		  [], [enable_fortran=no])
    AM_CONDITIONAL(ENABLE_FORTRAN, test "x$enable_fortran" = xyes)

    if test "x$enable_fortran" = xyes
    then
        AC_PROG_F77
        AC_F77_LIBRARY_LDFLAGS
    fi
    AC_PROG_CC dnl bux fix to restore $ac_ext
])
