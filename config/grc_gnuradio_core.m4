dnl Copyright 2001,2002,2003,2004,2005,2006 Free Software Foundation, Inc.
dnl 
dnl This file is part of GNU Radio
dnl 
dnl GNU Radio is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2, or (at your option)
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

AC_DEFUN([GRC_GNURADIO_CORE],[
    GRC_ENABLE([gnuradio-core])

    AC_CONFIG_FILES([ \
        gnuradio-core/Makefile
        gnuradio-core/gnuradio-core.pc \
        gnuradio-core/doc/Doxyfile \
        gnuradio-core/doc/Makefile \
        gnuradio-core/doc/other/Makefile \
        gnuradio-core/doc/xml/Makefile \
        gnuradio-core/src/Makefile \
        gnuradio-core/src/gen_interpolator_taps/Makefile \
        gnuradio-core/src/lib/Makefile \
        gnuradio-core/src/lib/filter/Makefile \
        gnuradio-core/src/lib/g72x/Makefile \
        gnuradio-core/src/lib/general/Makefile \
        gnuradio-core/src/lib/general/gr_prefix.cc \
        gnuradio-core/src/lib/gengen/Makefile \
        gnuradio-core/src/lib/io/Makefile \
        gnuradio-core/src/lib/missing/Makefile \
        gnuradio-core/src/lib/omnithread/Makefile \
        gnuradio-core/src/lib/reed-solomon/Makefile \
        gnuradio-core/src/lib/runtime/Makefile \
        gnuradio-core/src/lib/swig/Makefile \
        gnuradio-core/src/python/Makefile \
        gnuradio-core/src/python/bin/Makefile \
        gnuradio-core/src/python/gnuradio/Makefile \
        gnuradio-core/src/python/gnuradio/blks/Makefile \
        gnuradio-core/src/python/gnuradio/blksimpl/Makefile \
        gnuradio-core/src/python/gnuradio/gr/Makefile \
        gnuradio-core/src/python/gnuradio/gr/run_tests \
        gnuradio-core/src/python/gnuradio/gru/Makefile \
        gnuradio-core/src/python/gnuradio/gruimpl/Makefile \
        gnuradio-core/src/tests/Makefile \
        gnuradio-core/src/utils/Makefile \
    ])
    
    passed=yes
    GRC_BUILD_CONDITIONAL([gnuradio-core],[
        dnl run_tests is created from run_tests.in.  Make it executable.
        AC_CONFIG_COMMANDS([run_tests_core], [chmod +x gnuradio-core/src/python/gnuradio/gr/run_tests])

        dnl kludge up initial swig dependency files
        AC_CONFIG_COMMANDS([swig_deps],	[
		touch gnuradio-core/src/lib/swig/gnuradio_swig_py_runtime.d
		touch gnuradio-core/src/lib/swig/gnuradio_swig_py_general.d
		touch gnuradio-core/src/lib/swig/gnuradio_swig_py_gengen.d
		touch gnuradio-core/src/lib/swig/gnuradio_swig_py_filter.d
		touch gnuradio-core/src/lib/swig/gnuradio_swig_py_io.d
		])
    ])
])
