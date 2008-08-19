dnl
dnl Copyright 2005,2008 Free Software Foundation, Inc.
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

# GR_LIB64()
#
# Checks to see if we're on a x86_64 or powerpc64 machine, and if so, detemine
# if libdir should end in "64" or not.
# 
# May append "64" to libdir.
# Sets gr_libdir_suffix to "" or "64"
#
AC_DEFUN([GR_LIB64],[
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_REQUIRE([AC_PROG_CXX])

  # If we're using g++, extract the first SEARCH_DIR("...") entry from the linker script
  # and see if it contains a suffix after the final .../lib part of the path.
  # (This works because the linker script varies depending on whether we're generating
  # 32-bit or 64-bit executables)

  AC_MSG_CHECKING([libdir suffix])
  if test "$ac_cv_cxx_compiler_gnu" = "yes";
  then
    gr_libdir_suffix=`$CXX -Wl,--verbose 2>/dev/null | sed -n -e '/SEARCH_DIR/{s/;.*$//; s,^.*/,,; s/".*$//; s/^lib//; p}'`
  fi
  AC_MSG_RESULT([$gr_libdir_suffix])
  AC_SUBST(gr_libdir_suffix)

  if test "$host_cpu" = "x86_64" || test "$host_cpu" = "powerpc64" ; then
    AC_MSG_CHECKING([libdir for lib64 suffix])
    t=${libdir##*/lib}
    if test "$t" != 64 && test "$gr_libdir_suffix" = "64"; then
      libdir=${libdir}64
      AC_MSG_RESULT([no. Setting libdir to $libdir])
    else
      AC_MSG_RESULT([yes])
    fi
  fi
])
