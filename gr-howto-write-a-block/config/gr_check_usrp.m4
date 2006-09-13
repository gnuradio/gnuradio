dnl
dnl Copyright 2003 Free Software Foundation, Inc.
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

dnl Check for Universal Software Radio Peripheral

AC_DEFUN([GR_CHECK_USRP],[
  PKG_CHECK_MODULES(USRP, usrp >= 0.2,
	[HAVE_USRP=yes
	   AC_DEFINE(HAVE_USRP,[1],[Define if you have a USRP])],
	[HAVE_USRP=no])

  AM_CONDITIONAL(HAVE_USRP,     test x$HAVE_USRP = xyes)
])

