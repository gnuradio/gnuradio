dnl Copyright 2009 Free Software Foundation, Inc.
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


AC_DEFUN([GR_GIT],[
  dnl Identify git binary
  AC_PATH_PROG([GIT],[git])
  
  dnl If it exists, get either 'git describe' or fallback to current commit
  if test x$GIT != x ; then
    if (cd $srcdir && $GIT describe >/dev/null 2>&1); then
      GIT_VERSION=`cd $srcdir && $GIT describe --abbrev=8 | cut -f 2- -d '-'`
    else
      if (cd $srcdir && $GIT describe --always --abbrev=8 >/dev/null 2>&1); then
        GIT_VERSION=`cd $srcdir && $GIT describe --always --abbrev=8`
      fi
    fi
  fi
])
