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

AC_DEFUN([GRC_PMT],[
    GRC_ENABLE([pmt])
    GRC_WITH([pmt])
    passed=no
    if test x$with_pmt = xyes; then
        if test x$enable_pmt = xyes; then
	    AC_MSG_ERROR([Component pmt: Cannot use both --enable and --with])
        else
	    PKG_CHECK_MODULES(PMT, pmt, passed=with,
	    	   AC_MSG_RESULT([Component pmt: PKGCONFIG cannot find info]))
            if test x$passed = xwith; then
                pmt_INCLUDES=`$PKG_CONFIG --cflags-only-I pmt`
   	        pmt_LA=$PMT_LIBS
            fi
	fi
    fi
    dnl if $passed = with, then "--with" worked; ignore the "--enable" stuff
    dnl otherwise, $passed = no; check the "--enable" stuff
    if test x$passed = xno; then
        AC_CONFIG_FILES([\
            pmt/Makefile \
	    pmt/pmt.pc \
	    pmt/doc/Makefile \
	    pmt/src/Makefile \
	    pmt/src/lib/Makefile \
	    pmt/src/scheme/Makefile \
	    pmt/src/scheme/gnuradio/Makefile \
        ])
	passed=yes
	# Don't do pmt if omnithread skipped
	if test x$omnithread_skipped = xyes; then
	    AC_MSG_RESULT([Component pmt requires omnithread, which is not being built or specified via pre-installed files.])
	    passed=no
	fi
	# Don't do pmt if guile not available
        AC_PATH_PROG(GUILE,guile)
        if test "$GUILE" = "" ; then
            AC_MSG_RESULT([Component pmt requires guile, which was not found.])
            passed=no
        fi
	pmt_INCLUDES="-I\${abs_top_srcdir}/pmt/src/lib"
        pmt_LA="\${abs_top_builddir}/pmt/src/lib/libpmt.la"
    fi

    GRC_BUILD_CONDITIONAL([pmt],[
        dnl run_tests is created from run_tests.in.  Make it executable.
	dnl AC_CONFIG_COMMANDS([run_tests_pmt], [chmod +x pmt/src/python/run_tests])
    ])

    AC_SUBST([pmt_INCLUDES], [$pmt_INCLUDES])
    AC_SUBST([pmt_LA], [$pmt_LA])
])
