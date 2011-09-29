dnl Copyright 2001,2002,2003,2004,2005,2006,2008,2010 Free Software Foundation, Inc.
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

AC_DEFUN([GRC_GR_TRELLIS],[
    GRC_ENABLE(gr-trellis)

    dnl Don't do gr-trellis if gnuradio-core skipped
    GRC_CHECK_DEPENDENCY(gr-trellis, gnuradio-core)
    GRC_CHECK_DEPENDENCY(gr-trellis, gr-digital)

    AC_CONFIG_FILES([\
        gr-trellis/Makefile \
	gr-trellis/gnuradio-trellis.pc \
        gr-trellis/doc/Makefile \
        gr-trellis/grc/Makefile \
        gr-trellis/src/Makefile \
        gr-trellis/src/lib/Makefile \
        gr-trellis/src/lib/run_guile_tests \
        gr-trellis/src/python/Makefile \
        gr-trellis/src/python/run_tests \
        gr-trellis/src/examples/Makefile \
        gr-trellis/src/examples/fsm_files/Makefile
    ])

    GRC_BUILD_CONDITIONAL(gr-trellis,[
        dnl run_tests is created from run_tests.in.  Make it executable.
	AC_CONFIG_COMMANDS([run_tests_gr_trellis],
	                   [chmod +x gr-trellis/src/python/run_tests;
			    chmod +x gr-trellis/src/lib/run_guile_tests])
    ])
])
