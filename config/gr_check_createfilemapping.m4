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

dnl AC_DEFUN([GR_CHECK_CREATEFILEMAPPING],
dnl [
dnl   AC_CHECK_FUNCS([CreateFileMapping])
dnl ])

AC_DEFUN([GR_CHECK_CREATEFILEMAPPING],[
  AC_MSG_CHECKING([for CreateFileMapping function])
  AC_COMPILE_IFELSE([
#include <windows.h>
int main (int argc, char **argv)
{ 
    HANDLE handle;
    int size;
    char	 	seg_name[[1024]];
    handle = CreateFileMapping(
                     INVALID_HANDLE_VALUE,    // use paging file
                     NULL,                    // default security
                     PAGE_READWRITE,          // read/write access
                     0,                       // max. object size
                     size,                // buffer size
                     seg_name);                 // name of mapping object
    return 0;
}
],[HAVE_CREATEFILEMAPPING=yes
   AC_DEFINE(HAVE_CREATEFILEMAPPING,[1],[Define if you have the CreateFilemapping function(win32).])],
  [HAVE_CREATEFILEMAPPING=no])

  AC_MSG_RESULT($HAVE_CREATEFILEMAPPING)
  AM_CONDITIONAL(HAVE_CREATEFILEMAPPING,     test x$HAVE_CREATEFILEMAPPING = xyes)
])


