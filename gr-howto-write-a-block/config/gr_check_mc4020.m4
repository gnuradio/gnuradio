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

AC_DEFUN([GR_CHECK_MC4020],[
  AC_MSG_CHECKING([for mc4020 A/D driver include file])
  AC_COMPILE_IFELSE([
#include <mc4020.h>
int main (int argc, char **argv)
{ 
    return 0;
}
],[HAVE_MC4020=yes
   AC_DEFINE(HAVE_MC4020,[1],[Define if you have a Measurement Computing PCI-DAS4020/12 A/D])],
  [HAVE_MC4020=no])

  AC_MSG_RESULT($HAVE_MC4020)
  AM_CONDITIONAL(HAVE_MC4020,     test x$HAVE_MC4020 = xyes)
])

