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

AC_DEFUN([GRC_GRC],[
    GRC_ENABLE(grc)

    GRC_CHECK_DEPENDENCY(grc, gr-wxgui)

    AC_CHECK_PROG(XDG_UTILS, xdg-mime, true, false)
    AM_CONDITIONAL(XDG_UTILS, $XDG_UTILS)

    dnl ########################################
    dnl # test python version
    dnl ########################################
    AC_MSG_CHECKING([for Python version >= 2.5])
    result=`${PYTHON} -c "import sys; print sys.version.split()[[0]] >= '2.5'"`
    if test "$result" = "True"; then
        AC_MSG_RESULT([yes])
    else
        AC_MSG_RESULT([no])
        passed=no
    fi

    dnl ########################################
    dnl # test python dependencies
    dnl ########################################
    if test $passed = yes; then
        PYTHON_CHECK_MODULE([Cheetah],[Python Cheetah templates],[],[passed=no])
        PYTHON_CHECK_MODULE([lxml],[Python XML wrappers],[],[passed=no])
        PYTHON_CHECK_MODULE([pygtk],[Python GTK wrappers],[],[passed=no])
    fi

    dnl ########################################
    dnl # test gtk version
    dnl ########################################
    if test $passed = yes; then
        AC_MSG_CHECKING([for Python GTK version >= 2.10.0])
        result=`${PYTHON} -c "import gtk; print gtk.pygtk_version >= (2, 10, 0)" 2> /dev/null`
        if test "$result" = "True"; then
            AC_MSG_RESULT([yes])
        else
            AC_MSG_RESULT([no])
            passed=no
        fi
    fi

    dnl ########################################
    dnl # platform dependency pythonw
    dnl ########################################
    dnl FIXME: move this test to Python config m4
    if test `${PYTHON} -c "import sys; print sys.platform"` = 'darwin'; then
        PYTHONW=pythonw
    else
        PYTHONW=python
    fi
    AC_SUBST(PYTHONW)

    AC_CONFIG_FILES([ \
        grc/src/grc_gnuradio/Makefile \
        grc/src/grc_gnuradio/wxgui/Makefile \
        grc/src/grc_gnuradio/blks2/Makefile \
        grc/src/grc_gnuradio/usrp/Makefile \
        grc/src/Makefile \
        grc/src/platforms/Makefile \
        grc/src/platforms/base/Makefile \
        grc/src/platforms/python/Makefile \
        grc/src/platforms/python/utils/Makefile \
        grc/src/platforms/gui/Makefile \
        grc/src/utils/Makefile \
        grc/src/gui/Makefile \
        grc/examples/Makefile \
        grc/examples/xmlrpc/Makefile \
        grc/examples/usrp/Makefile \
        grc/examples/audio/Makefile \
        grc/examples/simple/Makefile \
        grc/examples/trellis/Makefile \
        grc/Makefile \
        grc/data/Makefile \
        grc/data/platforms/Makefile \
        grc/data/platforms/base/Makefile \
        grc/data/platforms/python/Makefile \
        grc/data/platforms/python/blocks/Makefile \
        grc/scripts/Makefile \
        grc/freedesktop/Makefile \
    ])

    GRC_BUILD_CONDITIONAL(grc)
])
