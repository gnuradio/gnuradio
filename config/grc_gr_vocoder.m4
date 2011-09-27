dnl Copyright 2011 Free Software Foundation, Inc.
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

AC_DEFUN([GRC_GR_VOCODER],[
    GRC_ENABLE(gr-vocoder)

    dnl Don't do gr-vocoder if gnuradio-core skipped
    GRC_CHECK_DEPENDENCY(gr-fr-vocoder, gnuradio-core)

    AC_CONFIG_FILES([\
        gr-vocoder/Makefile \
	gr-vocoder/gnuradio-vocoder.pc \
        gr-vocoder/apps/Makefile \
        gr-vocoder/doc/Makefile \
        gr-vocoder/examples/Makefile \
        gr-vocoder/grc/Makefile \
        gr-vocoder/include/Makefile \
        gr-vocoder/lib/Makefile \
	gr-vocoder/lib/codec2/Makefile \
	gr-vocoder/lib/g7xx/Makefile \
	gr-vocoder/lib/gsm/Makefile \
        gr-vocoder/python/Makefile \
	gr-vocoder/python/run_tests \
	gr-vocoder/swig/Makefile \
	gr-vocoder/swig/run_guile_tests \
    ])

    GRC_BUILD_CONDITIONAL(gr-vocoder,[
        dnl run_tests is created from run_tests.in.  Make it executable.
        AC_CONFIG_COMMANDS([run_tests_vocoder], [chmod +x gr-vocoder/python/run_tests])
	AC_CONFIG_COMMANDS([run_tests_vocoder_guile], [chmod +x gr-vocoder/swig/run_guile_tests])
    ])
])
