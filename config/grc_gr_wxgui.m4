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
    GRC_ENABLE(gr-wxgui)
    GRC_WITH(gr-wxgui)

    dnl Don't do gr-wxgui if gnuradio-core skipped
    GRC_CHECK_DEPENDENCY(gr-wxgui, gnuradio-core)

    dnl If execution gets to here, $passed will be:
    dnl   with : if the --with code didn't error out
    dnl   yes  : if the --enable code passed muster and all dependencies are met
    dnl   no   : otherwise
    if test $passed = yes; then
        PYTHON_CHECK_MODULE([wx],[Python wxWidgets wrappers >= 2.8],[],[passed=no],[wx.version().split()[[0]] >= "2.8"])
        PYTHON_CHECK_MODULE([numpy],[Numeric Python extensions],[],[passed=no])
    fi

    AC_CONFIG_FILES([ \
        gr-wxgui/Makefile \
        gr-wxgui/gr-wxgui.pc \
        gr-wxgui/grc/Makefile \
        gr-wxgui/src/Makefile \
        gr-wxgui/src/python/Makefile \
        gr-wxgui/src/python/plotter/Makefile \
    ])

    GRC_BUILD_CONDITIONAL(gr-wxgui)
])
