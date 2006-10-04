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

AC_DEFUN([GR_SET_MD_CPU],[
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_ARG_WITH(md-cpu,
	[  --with-md-cpu=ARCH      set machine dependent speedups (auto)],
		[cf_with_md_cpu="$withval"],
		[cf_with_md_cpu="$host_cpu"])

  AC_MSG_CHECKING([for machine dependent speedups])
  case "$cf_with_md_cpu" in
   x86 | i[[3-7]]86)	MD_CPU=x86	MD_SUBCPU=x86	;;
   x86_64)		MD_CPU=x86	MD_SUBCPU=x86_64	;;
#  sparc)       MD_CPU=sparc	 ;;
   *)           MD_CPU=generic   ;;
  esac
  AC_MSG_RESULT($MD_CPU)
  AC_SUBST(MD_CPU)
  AC_SUBST(MD_SUBCPU) 

  AM_CONDITIONAL(MD_CPU_x86,     test "$MD_CPU" = "x86")
  AM_CONDITIONAL(MD_SUBCPU_x86_64,  test "$MD_SUBCPU" = "x86_64")
  AM_CONDITIONAL(MD_CPU_generic, test "$MD_CPU" = "generic")
])
