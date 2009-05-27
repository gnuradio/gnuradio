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

AC_DEFUN([GRC_GR_ATSC],[
    GRC_ENABLE(gr-atsc)

    dnl Don't do gr-atsc if gnuradio-core skipped
    GRC_CHECK_DEPENDENCY(gr-atsc, gnuradio-core)

    AC_CONFIG_FILES([\
        gr-atsc/Makefile \
	gr-atsc/gnuradio-atsc.pc \
        gr-atsc/doc/Makefile \
        gr-atsc/src/Makefile \
        gr-atsc/src/lib/Makefile \
        gr-atsc/src/python/Makefile \
        gr-atsc/src/python/run_tests \
    ])

    GRC_BUILD_CONDITIONAL(gr-atsc,[
        dnl run_tests is created from run_tests.in.  Make it executable.
	AC_CONFIG_COMMANDS([run_tests_atsc], [chmod +x gr-atsc/src/python/run_tests])
    ])
])
