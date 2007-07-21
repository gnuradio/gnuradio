dnl
dnl Copyright 2007 Free Software Foundation, Inc.
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

dnl Configure paths for library qwt.
dnl
dnl GR_QWT([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
dnl Test for library qwt, set QWT_CFLAGS and QWT_LIBS if found. Assumes 
dnl QT_CFLAGS and QT_LIBS have already been correctly set.
dnl

AC_DEFUN([GR_QWT],
[
    dnl Save the environment
    AC_LANG_PUSH(C++)
    qwt_save_CPPFLAGS="$CPPFLAGS"
    qwt_save_LIBS="$LIBS"
    libqwt_ok=yes

    dnl Allow user to specify where QWT files are
    AC_ARG_WITH([qwt-libdir],
		[  --with-qwt-libdir=path  Prefix where QWT library is installed (optional)],
		[qwt_libdir="$withval"], [qwt_libdir=""])

    AC_ARG_WITH([qwt-incdir],
		[  --with-qwt-incdir=path  Prefix where QWT include files are (optional)],
		[qwt_incdir="$withval"], [qwt_incdir=""])

    dnl Create QWT_CFLAGS based on user input
    AC_MSG_CHECKING(QWT_CFLAGS)
    if test "$qwt_incdir" != "" ; then
	QWT_CFLAGS="$QWT_CFLAGS -I$qwt_incdir"
    fi
    AC_MSG_RESULT($QWT_CFLAGS)
    
    dnl Set CPPFLAGS so C++ tests can operate
    CPPFLAGS="$CPPFLAGS $QT_CFLAGS $QWT_CFLAGS"

    dnl Check for presence of header files
    AC_CHECK_HEADERS([qwt.h], 
		     [],
		     [libqwt_ok=no;AC_MSG_RESULT([cannot find usable qwt headers])]
    )

    dnl Set QWT_LIBS based on user input
    AC_MSG_CHECKING(QWT_LIBS)
    QWT_LIBS="$QWT_LIBS -lqwt"
    if test "$qwt_libdir" != "" ; then
	QWT_LIBS="-L$qwt_libdir $QWT_LIBS"
    fi
    AC_MSG_RESULT($QWT_LIBS)

    dnl Set LIBS so C++ link test can operate
    LIBS="$QWT_LIBS $QT_LIBS $LIBS"

    dnl Check that library files can be linked in
    dnl This references an arbitrary static class method 
    AC_TRY_LINK([#include <qwt_text.h>],
		[QwtTextEngine const *te = QwtText::textEngine(QwtText::AutoText)],
		[],
                [libqwt_ok=no;AC_MSG_RESULT([unable to link QWT library])]
    )

    dnl Restore saved variables
    LIBS="$qwt_save_LIBS"
    CPPFLAGS="$qwt_save_CPPFLAGS"
    AC_LANG_POP

    dnl Execute user actions
    if test "x$libqwt_ok" = "xyes" ; then
	ifelse([$1], , :, [$1])
    else
	QWT_CFLAGS=""
	QWT_LIBDIRS=""
	ifelse([$2], , :, [$2])
    fi

    dnl Export our variables
    AC_SUBST(QWT_CFLAGS)
    AC_SUBST(QWT_LIBS)
])
