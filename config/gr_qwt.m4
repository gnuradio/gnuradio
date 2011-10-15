dnl
dnl Copyright 2007,2008 Free Software Foundation, Inc.
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
    dnl QWT Library Version
    QWT_LIBRARY1=-lqwt-qt4
    QWT_LIBRARY2=-lqwt

    dnl Save the environment
    AC_LANG_PUSH(C++)
    qwt_save_CPPFLAGS="$CPPFLAGS"
    qwt_save_LIBS="$LIBS"
    libqwt_ok=yes

    dnl QWT Info
    dnl Allow user to specify where QWT files are
    AC_ARG_WITH([qwt-libdir],
		[  --with-qwt-libdir=path  Prefix where QWT library is installed (optional)],
		[qwt_libdir="$withval"], [qwt_libdir=""])

    AC_ARG_WITH([qwt-incdir],
		[  --with-qwt-incdir=path  Prefix where QWT include files are (optional)],
		[qwt_incdir="$withval"], [qwt_incdir=""])
    AC_ARG_WITH([qwt-lib],
		[  --with-qwt-lib=library  QWT library name (optional)],
		[qwt_lib="$withval"], [qwt_lib=""])

    dnl Check for presence of header files
    dnl if not user-specified, try the first include dir (Ubuntu), then
    dnl try the second include dir (Fedora)
    CPPFLAGS="$CPPFLAGS $QTCORE_CFLAGS"

    dnl if not set by user
    if test "$qwt_incdir" = "" ; then
        dnl check qwt-qt4/qwt.h (as in Ubuntu)
        AC_CHECK_HEADER(
            [qwt-qt4/qwt_math.h],
            [qwt_qt4_qwt_h=yes],
            [qwt_qt4_qwt_h=no]
        )
        dnl If it was found, set the flags and move on
        if test "$qwt_qt4_qwt_h" = "yes" ; then
            QWT_CFLAGS="$QWT_CFLAGS -I/usr/include/qwt-qt4"
        else
            dnl check qwt/qwt.h (as in Fedora)
            AC_CHECK_HEADER(
                [qwt/qwt_math.h],
	        [qwt_qwt_h=yes],
                [qwt_qwt_h=no]
            )
            dnl if it was found, set the flags and move on
            if test "$qwt_qwt_h" = "yes" ; then
                QWT_CFLAGS="$QWT_CFLAGS -I/usr/include/qwt"
            else
                dnl otherwise, qwt.h wasn't found, so set the flag to no
                libqwt_ok=no
            fi
        fi
    else
	dnl Using the user-specified include directory
	QWT_CFLAGS="$QWT_CFLAGS -I$qwt_incdir"
        AC_CHECK_HEADER(
            [$qwt_incdir/qwt.h],
	    [],
	    [libqwt_ok=no])
    fi

    dnl Don't bother going on if we can't find the headers
    if test "$libqwt_ok" = "yes" ; then

        dnl Check for QWT library (qwt or qwt-qt4)

        dnl User-defined QWT library path
        if test "$qwt_libdir" != "" ; then
            QWT_LIBS="-L$qwt_libdir $QWT_LIBS"
        fi

        dnl temporarily set these so the AC_CHECK_LIB works
        CPPFLAGS="$CPPFLAGS $QWT_CFLAGS"
        LIBS="$qwt_save_LIBS $QT_LIBS $QWT_LIBS -lqwt"

        dnl If the user specified a qwt library name, use it here
        if test "$qwt_lib" != "" ; then
            AC_CHECK_LIB([$qwt_lib], [main], [libqwt_ok=yes], [libqwt_ok=no])

        else            
            dnl Check for 'main' in libqwt (Fedora)
            AC_CHECK_LIB([qwt], [main], [libqwt_ok=yes], [libqwt_ok=no])

            dnl If library found properly, set the flag and move on
            if test "$libqwt_ok" = "yes" ; then
                QWT_LIBS="$QWT_LIBS -lqwt"
            else
                dnl Otherwise, check for 'main' in libqwt-qt4 (Ubuntu)
                LIBS="$qwt_save_LIBS $QT_LIBS $QWT_LIBS -lqwt-qt4"
                AC_CHECK_LIB([qwt-qt4], [main], [libqwt_ok=yes], [libqwt_ok=no])
                if test "$libqwt_ok" = "yes" ; then
                    QWT_LIBS="$QWT_LIBS -lqwt-qt4"
                else
                    AC_MSG_RESULT([Could not link to libqwt.so])
                fi
            fi
        fi
    else
        AC_MSG_RESULT([Could not find qwt headers])
    fi

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
