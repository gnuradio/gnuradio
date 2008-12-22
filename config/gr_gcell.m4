#
# Copyright 2008 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

AC_DEFUN([GR_GCELL],[
  passed=yes
  PKG_CHECK_MODULES(GCELL, gcell >= 3.1, [],
    [passed=no; AC_MSG_RESULT([gcell not found])])
  PKG_CHECK_MODULES(GCELL_SPU, gcell_spu >= 3.1, [],
    [passed=no; AC_MSG_RESULT([gcell_spu not found])])

  GCELL_EMBEDSPU_LIBTOOL=
  if test $passed = yes; then
    GCELL_EMBEDSPU_LIBTOOL=`$PKG_CONFIG --variable=gcell_embedspu_libtool gcell 2>/dev/null`
    AC_DEFINE(HAVE_GCELL,[1],[Define if you have gcell installed])
  fi
  AC_SUBST([GCELL_EMBEDSPU_LIBTOOL])

  AM_CONDITIONAL([HAVE_GCELL], [test "$passed" = "yes"])
])
