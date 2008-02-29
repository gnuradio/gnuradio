dnl Copyright 2001,2002,2003,2004,2005,2006,2007,2008 Free Software Foundation, Inc.
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

AC_DEFUN([GRC_OMNITHREAD],[
    GRC_ENABLE(omnithread)

    GRC_WITH(omnithread, [], gnuradio-omnithread)

    dnl If execution gets to here, $passed will be:
    dnl   with : if the --with code didn't error out
    dnl   yes  : if the --enable code passed muster and all dependencies are met
    dnl   no   : otherwise
    if test $passed != with; then
	dnl how and where to find INCLUDES and LA and such
        omnithread_INCLUDES="-I\${abs_top_srcdir}/omnithread"
        omnithread_LA="\${abs_top_builddir}/omnithread/libgromnithread.la"
	omnithread_LIBDIRPATH="\${abs_top_builddir}/omnithread:\${abs_top_builddir}/omnithread/.libs"
    fi

    AC_CONFIG_FILES([ \
        omnithread/Makefile \
        omnithread/gnuradio-omnithread.pc
    ])

    GRC_BUILD_CONDITIONAL(omnithread,[
        dnl run_tests is created from run_tests.in.  Make it executable.
        dnl AC_CONFIG_COMMANDS([run_tests_omnithread], [chmod +x omnithread/run_tests])
    ])
])
