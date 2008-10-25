dnl
dnl Copyright 2008 Free Software Foundation, Inc.
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

dnl Configure paths for library qwtplot3d.
dnl
dnl GR_QWTPLOT3D([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
dnl Test for library qwtplot3d, set QWTPLOT3D_CFLAGS and QWTPLOT3D_LIBS if found. Assumes 
dnl QT_CFLAGS and QT_LIBS have already been correctly set.
dnl

AC_DEFUN([GR_QWTPLOT3D],
[
    dnl QWTPLOT3D Library Version

    dnl Save the environment
    AC_LANG_PUSH(C++)
    qwtplot3d_save_CPPFLAGS="$CPPFLAGS"
    qwtplot3d_save_LIBS="$LIBS"
    libqwtplot3d_ok=yes

    dnl Allow user to specify where QWTPLOT3D files are
    AC_ARG_WITH([qwtplot3d-libdir],
		[  --with-qwtplot3d-libdir=path  Prefix where QWTPLOT3D library is installed (optional)],
		[qwtplot3d_libdir="$withval"], [qwtplot3d_libdir=""])

    AC_ARG_WITH([qwtplot3d-incdir],
		[  --with-qwtplot3d-incdir=path  Prefix where QWTPLOT3D include files are (optional)],
		[qwtplot3d_incdir="$withval"], [qwtplot3d_incdir=""])

    AC_ARG_WITH([qwtplot3d-lib],
		[  --with-qwtplot3d-lib=library  QWT Plot3D library name (optional)],
		[qwtplot3d_lib="$withval"], [qwtplot3d_lib=""])


    dnl Check for presence of header files
    dnl if not user-specified, try the first include dir (Ubuntu), then
    dnl try the second include dir (Fedora)
    CPPFLAGS="$CPPFLAGS $QTCORE_CFLAGS $QTGUI_CFLAGS $QWT_CFLAGS"

    dnl if not set by user
    if test "$qwtplot3d_incdir" = "" ; then
        dnl check qwtplot3d/qwtplot3d.h (as in Fedora)
        AC_CHECK_HEADER(
            [qwtplot3d/qwt3d_plot.h],
	    [qwtplot3d_qwtplot3d_h=yes],
            [qwtplot3d_qwtplot3d_h=no]
        )
        dnl If it was found, set the flags and move on
        if test "$qwtplot3d_qwtplot3d_h" = "yes" ; then
            QWTPLOT3D_CFLAGS="$QWTPLOT3D_CFLAGS -I/usr/include/qwtplot3d"
        else
            dnl otherwise, check qwtplot3d-qt4/qwtplot3d.h (as in Ubuntu)
            AC_CHECK_HEADER(
                [qwtplot3d-qt4/qwt3d_plot.h],
                [qwtplot3d_qt4_qwtplot3d_h=yes],
                [qwtplot3d_qt4_qwtplot3d_h=no]
            )
            dnl if it was found, set the flags and move on
            if test "$qwtplot3d_qt4_qwtplot3d_h" = "yes" ; then
                QWTPLOT3D_CFLAGS="$QWTPLOT3D_CFLAGS -I/usr/include/qwtplot3d-qt4"
            else
                dnl otherwise, qwtplot3d.h wasn't found, so set the flag to no
                libqwtplot3d_ok=no
            fi
        fi
    else
	dnl Using the user-specified include directory
	QWTPLOT3D_CFLAGS="$QWTPLOT3D_CFLAGS -I$qwtplot3d_incdir"
        AC_CHECK_HEADER(
            [$qwtplot3d_incdir/qwt3d_plot.h],
	    [],
	    [libqwtplot3d_ok=no])
    fi

    dnl Don't bother going on if we can't find the headers
    if test "$libqwtplot3d_ok" = "yes" ; then

        dnl Check for QWTPLOT3D library (qwtplot3d or qwtplot3d-qt4)

        dnl User-defined QWTPLOT3D library path
        if test "$qwtplot3d_libdir" != "" ; then
            QWTPLOT3D_LIBS="-L$qwtplot3d_libdir $QWTPLOT3D_LIBS"
        fi

        dnl temporarily set these so the AC_CHECK_LIB works
        CPPFLAGS="$CPPFLAGS $QWTPLOT3D_CFLAGS"
        LIBS="$qwtplot3d_save_LIBS $QT_LIBS $QWTPLOT3D_LIBS -lqwtplot3d-qt4"

        dnl If the user specified a qwtplot3d library name, use it here
        if test "$qwtplot3d_lib" != "" ; then
            AC_CHECK_LIB([$qwtplot3d_lib], [main], [libqwtplot3d_ok=yes], [libqwtplot3d_ok=no])

        else            
            dnl Check for 'main' in libqwtplot3d-qt4
            AC_CHECK_LIB([qwtplot3d-qt4], [main], [libqwtplot3d_ok=yes], [libqwtplot3d_ok=no])

            dnl If library found properly, set the flag and move on
            if test "$libqwtplot3d_ok" = "yes" ; then
                QWTPLOT3D_LIBS="$QWTPLOT3D_LIBS -lqwtplot3d-qt4"
            else
                dnl Otherwise, check for 'main' in libqwtplot3d
                LIBS="$qwtplot3d_save_LIBS $QT_LIBS $QWTPLOT3D_LIBS -lqwtplot3d"
                AC_CHECK_LIB([qwtplot3d], [main], [libqwtplot3d_ok=yes], [libqwtplot3d_ok=no])
                if test "$libqwtplot3d_ok" = "yes" ; then
                    QWTPLOT3D_LIBS="$QWTPLOT3D_LIBS -lqwtplot3d"
                else
                    AC_MSG_RESULT([Could not link to libqwtplot3d.so])
                fi
            fi
        fi
    else
        AC_MSG_RESULT([Could not find qwtplot3d headers])
    fi

    dnl Restore saved variables
    LIBS="$qwtplot3d_save_LIBS"
    CPPFLAGS="$qwtplot3d_save_CPPFLAGS"
    AC_LANG_POP

    dnl Execute user actions
    if test "x$libqwtplot3d_ok" = "xyes" ; then
	ifelse([$1], , :, [$1])
    else
	QWTPLOT3D_CFLAGS=""
	QWTPLOT3D_LIBDIRS=""
	ifelse([$2], , :, [$2])
    fi

    dnl Export our variables
    AC_SUBST(QWTPLOT3D_CFLAGS)
    AC_SUBST(QWTPLOT3D_LIBS)
])
