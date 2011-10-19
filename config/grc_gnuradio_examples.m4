dnl Copyright 2001,2002,2003,2004,2005,2006,2008,2009,2011 Free Software Foundation, Inc.
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

AC_DEFUN([GRC_GNURADIO_EXAMPLES],[
    GRC_ENABLE(gnuradio-examples)

    dnl Do not do gnuradio-examples if gnuradio-core or gr-audio skipped
    GRC_CHECK_DEPENDENCY(gnuradio-examples, gnuradio-core)
    GRC_CHECK_DEPENDENCY(gnuradio-examples, gr-audio)

    AC_CONFIG_FILES([ \
        gnuradio-examples/Makefile \
	gnuradio-examples/python/Makefile \
	gnuradio-examples/grc/Makefile \
   	gnuradio-examples/python/mp-sched/Makefile \
   	gnuradio-examples/python/network/Makefile \
   	gnuradio-examples/python/pfb/Makefile \
   	gnuradio-examples/python/tags/Makefile \
	gnuradio-examples/waveforms/Makefile \
    ])

    GRC_BUILD_CONDITIONAL(gnuradio-examples)
])
