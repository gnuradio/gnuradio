dnl Copyright 2012 Free Software Foundation, Inc.
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

AC_DEFUN([GRC_GR_FCD],[
    GRC_ENABLE(gr-fcd)

    dnl Dont do gr-fcd if gnuradio-core or gr-audio skipped
    GRC_CHECK_DEPENDENCY(gr-fcd, gnuradio-core)
    GRC_CHECK_DEPENDENCY(gr-fcd, gr-audio)

    FCD_MACOSX="no"
    FCD_WINDOWS="no"

    if test $passed = yes; then
       PKG_CHECK_MODULES(USB, libusb-1.0 >= 1.0, [], [passed=no; AC_MSG_RESULT([gr-fcd requires libusb-1.0 >= 1.0])] )	 
       AC_SUBST(USB_CPPFLAGS)
       AC_SUBST(USB_LIBS)

       FCD_CPPFLAGS="${FCD_CPPFLAGS} -I\${abs_top_srcdir}/gr-fcd/include/fcd"
       AC_SUBST(FCD_CPPFLAGS)

       case "$target" in
       	    *-*-darwin*)
            FCD_MACOSX=yes
       ;;
     	    *-*-win*)
            FCD_WINDOWS=yes
       ;;
       esac
    fi

    AM_CONDITIONAL([FCD_MACOSX], [test $FCD_MACOSX = yes])
    AM_CONDITIONAL([FCD_WINDOWS], [test $FCD_WINDOWS = yes])

    # Use this to tell the Makefile whether to define
    # GR_HAVE_FCD for swig.
    AM_CONDITIONAL([GR_DEFINE_HAVE_FCD],[test $passed = yes])

    AC_CONFIG_FILES([ \
        gr-fcd/Makefile \
    	gr-fcd/gnuradio-fcd.pc \
	gr-fcd/include/Makefile \
	gr-fcd/include/fcd/Makefile \
	gr-fcd/lib/Makefile \
	gr-fcd/lib/hid/Makefile \
	gr-fcd/lib/fcd/Makefile \
    	gr-fcd/swig/Makefile \
	gr-fcd/swig/run_guile_tests \
    	gr-fcd/python/Makefile \
	gr-fcd/python/run_tests \
    	gr-fcd/grc/Makefile \
    	gr-fcd/examples/Makefile \
	gr-fcd/doc/Makefile \
    ])

    GRC_BUILD_CONDITIONAL(gr-fcd,[
	dnl run_tests is created from run_tests.in.  Make it executable.
	AC_CONFIG_COMMANDS([run_tests_fcd], [chmod +x gr-fcd/python/run_tests gr-fcd/swig/run_guile_tests])
    ])
])
