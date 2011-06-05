dnl Copyright 2008, 2009, 2011 Free Software Foundation, Inc.
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

AC_DEFUN([GRC_GRC],[
    GRC_ENABLE(grc)

    AC_CHECK_PROG(XDG_UTILS, xdg-mime, true, false)
    AM_CONDITIONAL(XDG_UTILS, $XDG_UTILS)

    dnl ########################################
    dnl # test python modules and versions
    dnl ########################################
    if test $passed = yes; then
        PYTHON_CHECK_MODULE([sys],[Python >= 2.5],[],[passed=no],[sys.version.split()[[0]] >= "2.5"])
        PYTHON_CHECK_MODULE([Cheetah],[Python Cheetah templates >= 2.0.0],[],[passed=no],[Cheetah.Version >= "2.0.0"])
        PYTHON_CHECK_MODULE([lxml.etree],[Python lxml wrappers >= 1.3.6],[],[passed=no],[lxml.etree.LXML_VERSION >= (1, 3, 6, 0)])
        PYTHON_CHECK_MODULE([gtk],[Python gtk wrappers >= 2.10.0],[],[passed=no],[gtk.pygtk_version >= (2, 10, 0)])
        PYTHON_CHECK_MODULE([numpy],[NumPy],[],[passed=no],[True])
    fi

    AC_CONFIG_FILES([ \
        grc/Makefile \
        grc/base/Makefile \
        grc/blocks/Makefile \
        grc/freedesktop/Makefile \
        grc/grc_gnuradio/Makefile \
        grc/gui/Makefile \
        grc/python/Makefile \
        grc/scripts/Makefile \
    ])

    GRC_BUILD_CONDITIONAL(grc)
])
