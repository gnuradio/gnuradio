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

AC_DEFUN([GRC_GNURADIO_CORE],[
    GRC_ENABLE([gnuradio-core])
    GRC_WITH([gnuradio-core])
    passed=no
    if test x$with_gnuradio_core = xyes; then
        if test x$enable_gnuradio_core = xyes; then
	    AC_MSG_ERROR([Component gnuradio-core: Cannot use both --enable and --with])
        else
	    PKG_CHECK_MODULES(GNURADIO_CORE, gnuradio-core, passed=with,
	    	   AC_MSG_RESULT([Component gnuradio-core: PKGCONFIG cannot find info]))
            if test x$passed = xwith; then
                gnuradio_core_INCLUDES=`$PKG_CONFIG --cflags-only-I gnuradio-core`
   	        gnuradio_core_LA=$GNURADIO_CORE_LIBS
	        gnuradio_core_INCLUDEDIR=`$PKG_CONFIG --variable=includedir gnuradio-core`
		gnuradio_core_SWIGDIRPATH="$gnuradio_core_INCLUDEDIR/swig"
		gnuradio_core_I="$gnuradio_core_SWIGDIRPATH/gnuradio.i"
		gnuradio_core_SWIG_INCLUDES="-I$gnuradio_core_SWIGDIRPATH"
		gnuradio_core_LIBDIRPATH=`$PKG_CONFIG --variable=libdir gnuradio-core`
		gnuradio_core_PYDIRPATH=$pythondir
            fi
	fi
    fi
    dnl if $passed = with, then "--with" worked; ignore the "--enable" stuff
    dnl otherwise, $passed = no; check the "--enable" stuff
    if test x$passed = xno; then
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
            gnuradio-core/src/lib/reed-solomon/Makefile \
            gnuradio-core/src/lib/runtime/Makefile \
            gnuradio-core/src/lib/swig/Makefile \
            gnuradio-core/src/python/Makefile \
            gnuradio-core/src/python/bin/Makefile \
            gnuradio-core/src/python/gnuradio/Makefile \
            gnuradio-core/src/python/gnuradio/blks2/Makefile \
            gnuradio-core/src/python/gnuradio/blks2impl/Makefile \
            gnuradio-core/src/python/gnuradio/gr/Makefile \
            gnuradio-core/src/python/gnuradio/gr/run_tests \
            gnuradio-core/src/python/gnuradio/gru/Makefile \
            gnuradio-core/src/python/gnuradio/gruimpl/Makefile \
	    gnuradio-core/src/python/gnuradio/vocoder/Makefile \
            gnuradio-core/src/tests/Makefile \
	    gnuradio-core/src/utils/Makefile \
        ])
        passed=yes

        # Don't do gnuradio-core if omnithread skipped
        if test x$omnithread_skipped = xyes; then
            AC_MSG_RESULT([Component gnuradio-core requires omnithread, which is not being built or specified via pre-installed files.])
	    passed=no
        fi
        gnuradio_core_INCLUDES="-I\${abs_top_srcdir}/gnuradio-core/src/lib/runtime \
		    -I\${abs_top_srcdir}/gnuradio-core/src/lib/general \
		    -I\${abs_top_builddir}/gnuradio-core/src/lib/general \
		    -I\${abs_top_srcdir}/gnuradio-core/src/lib/gengen \
		    -I\${abs_top_builddir}/gnuradio-core/src/lib/gengen \
		    -I\${abs_top_srcdir}/gnuradio-core/src/lib/filter \
		    -I\${abs_top_builddir}/gnuradio-core/src/lib/filter \
		    -I\${abs_top_srcdir}/gnuradio-core/src/lib/reed-solomon \
		    -I\${abs_top_srcdir}/gnuradio-core/src/lib/io \
		    -I\${abs_top_srcdir}/gnuradio-core/src/lib/g72x \
		    -I\${abs_top_srcdir}/gnuradio-core/src/lib/swig \
		    -I\${abs_top_builddir}/gnuradio-core/src/lib/swig \
	            \$(FFTW3F_CFLAGS)"
        gnuradio_core_LA="\${abs_top_builddir}/gnuradio-core/src/lib/libgnuradio-core.la"
	gnuradio_core_I="\${abs_top_srcdir}/gnuradio-core/src/lib/swig/gnuradio.i"
	gnuradio_core_LIBDIRPATH="\${abs_top_builddir}/gnuradio-core/src/lib:\${abs_top_builddir}/gnuradio-core/src/lib/.libs"
	gnuradio_core_SWIGDIRPATH="\${abs_top_builddir}/gnuradio-core/src/lib/swig:\${abs_top_builddir}/gnuradio-core/src/lib/swig/.libs:\${abs_top_srcdir}/gnuradio-core/src/lib/swig"
	gnuradio_core_PYDIRPATH="\${abs_top_srcdir}/gnuradio-core/src/python"
    fi
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
    AC_SUBST([gnuradio_core_INCLUDES])
    AC_SUBST([gnuradio_core_LA])
    AC_SUBST([gnuradio_core_I])
    AC_SUBST([gnuradio_core_LIBDIRPATH])
    AC_SUBST([gnuradio_core_SWIGDIRPATH])
    AC_SUBST([gnuradio_core_PYDIRPATH])
])
