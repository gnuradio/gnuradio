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
dnl the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
dnl Boston, MA 02111-1307, USA.

AC_DEFUN([GR_CHECK_SHM_OPEN],
[
  SHM_OPEN_LIBS=""
  save_LIBS="$LIBS"
  AC_SEARCH_LIBS([shm_open], [rt], [SHM_OPEN_LIBS="$LIBS"])
  AC_CHECK_FUNCS([shm_open])
  LIBS="$save_LIBS"
  AC_SUBST(SHM_OPEN_LIBS)
])
