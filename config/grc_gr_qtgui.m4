dnl Copyright 2001,2002,2003,2004,2005,2006 Free Software Foundation, Inc.
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

AC_DEFUN([GRC_GR_QTGUI],[
    GRC_ENABLE([gr-qtgui])

    AC_CONFIG_FILES([ \
	  gr-qtgui/Makefile \
	  gr-qtgui/src/Makefile \
	  gr-qtgui/src/lib/Makefile \
    ])

    # Check for package qt or qt-mt, set QT_CFLAGS and QT_LIBS
    passed=yes
    PKG_CHECK_MODULES(QT, qt >= 3.3,[],
    [passed=no;AC_MSG_RESULT([gr-qtgui requires libqt or libqt-mt, libqt not found. Checking for libqt-mt])])

    if test x$passed == xno; then
	passed=yes
	PKG_CHECK_MODULES(QT, qt-mt >= 3.3,[],
    	[passed=no;AC_MSG_RESULT([gr-qtgui requires libqt or libqt-mt, neither found.])])
    fi

    # Fetch QWT variables
    GR_QWT([], [passed=no])

    GRC_BUILD_CONDITIONAL([gr-qtgui],[])
])
