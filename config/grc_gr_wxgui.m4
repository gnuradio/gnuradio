dnl Copyright 2001,2002,2003,2004,2005,2006,2008 Free Software Foundation, Inc.
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

AC_DEFUN([GRC_GR_WXGUI],[
    GRC_ENABLE([gr-wxgui])
    GRC_WITH([gr-wxgui])
    passed=no
    if test x$with_gr_wxgui = xyes; then
        if test x$enable_gr_wxgui = xyes; then
	    AC_MSG_ERROR([Component gr-wxgui: Cannot use both --enable and --with])
        else
	    PKG_CHECK_MODULES(GR_WXGUI, gr-wxgui, passed=with,
	    	   AC_MSG_RESULT([Component gr-wxgui: PKGCONFIG cannot find info]))
	fi
    fi
    dnl if $passed = with, then "--with" worked; ignore the "--enable" stuff
    dnl otherwise, $passed = no; check the "--enable" stuff
    if test x$passed = xno; then
        AC_CONFIG_FILES([ \
	    gr-wxgui/Makefile \
	    gr-wxgui/gr-wxgui.pc \
	    gr-wxgui/src/Makefile \
	    gr-wxgui/src/python/Makefile \
        ])

        passed=yes
        # Don't do gr-wxgui if gnuradio-core skipped
        if test x$gnuradio_core_skipped = xyes; then
            AC_MSG_RESULT([Component gr-wxgui requires gnuradio-core, which is not being built or specified via pre-installed files.])
            passed=no
        fi
        # Don't do gr-wxgui if wxPython is not available
        if ! ${PYTHON} -c 'import wx'; then
            passed=no
        fi
    fi
    GRC_BUILD_CONDITIONAL([gr-wxgui])
])
