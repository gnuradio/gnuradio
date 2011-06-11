dnl
dnl Copyright 2011 Free Software Foundation, Inc.
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

AC_DEFUN([GR_VMCIRCBUF],[
  dnl Control availability of vmcircbuf methods.
  dnl For now, only allows disabling of shm methods, which cause uncatchable
  dnl   segmentation faults on Cygwin with gcc 4.x (x <= 5)

  AC_MSG_CHECKING([whether to try shm vmcircbuf methods])
  AC_ARG_WITH(shm-vmcircbuf,
              [AS_HELP_STRING([--without-shm-vmcircbuf],
                              [do not attempt to use shm vmcircbuf methods])],
              [], [ with_shm_vmcircbuf=yes ])
  AC_MSG_RESULT($with_shm_vmcircbuf)
  
  AS_IF([test "x$with_shm_vmcircbuf" != xno],
    [AC_DEFINE([TRY_SHM_VMCIRCBUF], [1],
               [Define if ok to try shm vmcircbuf methods]) ])
])
