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

AC_DEFUN([GRC_GR_SHD],[
    GRC_ENABLE(gr-shd)

    dnl Dont do gr-shd if gnuradio-core skipped
    GRC_CHECK_DEPENDENCY(gr-shd, gnuradio-core)

    if test $passed = yes; then
        dnl Don't do gr-shd if the 'shd' package is not installed
        PKG_CHECK_MODULES(
            [SHD], [shd >= 3.1.0 shd < 3.2.0], [],
            [passed=no; AC_MSG_RESULT([gr-shd requires libshd 3.1.x])]
        )
        SHD_CPPFLAGS="${SHD_CPPFLAGS} -I\${abs_top_srcdir}/gr-shd/include"
        AC_SUBST(SHD_CPPFLAGS)
        AC_SUBST(SHD_LIBS)

	# Use this to tell the Makefile whether to define
	# GR_HAVE_SHD for swig.
    fi

    AM_CONDITIONAL([GR_DEFINE_HAVE_SHD],[test $passed = yes])

    AC_CONFIG_FILES([ \
        gr-shd/gnuradio-shd.pc \
        gr-shd/Makefile \
        gr-shd/grc/Makefile \
        gr-shd/include/Makefile \
        gr-shd/lib/Makefile \
        gr-shd/swig/Makefile \
        gr-shd/swig/run_tests \
        gr-shd/swig/run_guile_tests \
        gr-shd/apps/Makefile \
    ])

    GRC_BUILD_CONDITIONAL(gr-shd,[
	dnl run_tests is created from run_tests.in.  Make it executable.
	AC_CONFIG_COMMANDS([run_tests_shd],
			   [chmod +x gr-shd/swig/run_tests gr-shd/swig/run_guile_tests])
    ])
])
