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

AC_DEFUN([GRC_USRP],[
    GRC_ENABLE([usrp])
    GRC_WITH([usrp])
    passed=no
    if test x$with_usrp = xyes; then
        if test x$enable_usrp = xyes; then
	    AC_MSG_ERROR([Component usrp: Cannot use both --enable and --with])
        else
	    # the following is required, for whatever reason; ignore the output
	    USRP_SET_FUSB_TECHNIQUE([],[passed=no;AC_MSG_RESULT([Unable to set fast USB technique.])])
    	    passed=no
	    # on to the regular routine
	    PKG_CHECK_MODULES(USRP, usrp, passed=with,
	    	    AC_MSG_RESULT([Component usrp: PKGCONFIG cannot find info]))
            if test x$passed = xwith; then
                usrp_INCLUDES=`$PKG_CONFIG --cflags-only-I usrp`
   	        usrp_LA=$USRP_LIBS
	        passed=no
	        PKG_CHECK_MODULES(USRP_INBAND, usrp-inband, passed=with,
	    	    AC_MSG_RESULT([Component usrp-inband: PKGCONFIG cannot find info]))
                if test x$passed = xwith; then
                    usrp_inband_INCLUDES=`$PKG_CONFIG --cflags-only-I usrp-inband`
   	            usrp_inband_LA=$USRP_INBAND_LIBS
		fi
            fi
	fi
    fi
    dnl if $passed = with, then "--with" worked; ignore the "--enable" stuff
    dnl otherwise, $passed = no; check the "--enable" stuff
    if test x$passed = xno; then
        AC_CONFIG_FILES([ \
    	    usrp/Makefile \
    	    usrp/usrp.pc \
    	    usrp/usrp-inband.pc \
            usrp/usrp.iss \
            usrp/doc/Doxyfile \
            usrp/doc/Makefile \
            usrp/doc/other/Makefile \
            usrp/host/Makefile \
            usrp/host/misc/Makefile \
            usrp/host/lib/Makefile \
            usrp/host/lib/inband/Makefile \
            usrp/host/lib/legacy/Makefile \
            usrp/host/lib/legacy/std_paths.h \
            usrp/host/swig/Makefile \
            usrp/host/apps/Makefile \
            usrp/host/apps-inband/Makefile \
            usrp/firmware/Makefile \
            usrp/firmware/include/Makefile \
            usrp/firmware/lib/Makefile \
            usrp/firmware/src/Makefile \
            usrp/firmware/src/common/Makefile \
            usrp/firmware/src/usrp2/Makefile \
            usrp/fpga/Makefile \
            usrp/fpga/rbf/Makefile \
        ])

        # gnulib.
        # FIXME: this needs to fail gracefully and continue, not implemented yet
        UTILS_FUNC_MKSTEMP

        # These checks don't fail
        AC_C_BIGENDIAN
        AC_CHECK_HEADERS([byteswap.h linux/compiler.h])
        AC_CHECK_FUNCS([getrusage sched_setscheduler pthread_setschedparam])
        AC_CHECK_FUNCS([sigaction snprintf])

	# There are 2 pkg-config files; the one for usrp requires omnithread
	# for Darwin only.
	case "$host_os" in
	    darwin*)
	        # for usrp.pc.in
	        usrp_darwin_omnithread_pc_requires="gnuradio-omnithread"
	        usrp_darwin_omnithread_pc_la="-lgromnithread"
      		;;
	    *)
	        # for usrp.pc.in (blanks)
	        usrp_darwin_omnithread_pc_requires=""
	        usrp_darwin_omnithread_pc_la=""
	        ;;
        esac
        passed=yes
	# Don't do usrp if omnithread, mblock, or pmt skipped
	if test x$omnithread_skipped = xyes; then
	     AC_MSG_RESULT([Component usrp requires omnithread, which is not being built or specified via pre-installed files.])
	    passed=no
	fi
        if test x$mblock_skipped = xyes; then
            AC_MSG_RESULT([Component usrp requires mblock, which is not being built or specified via pre-installed files.])
            passed=no
        fi
        if test x$pmt_skipped = xyes; then
            AC_MSG_RESULT([Component usrp requires pmt, which is not being built or specified via pre-installed files.])
            passed=no
        fi
	# Don't do mblock if guile not available (inband requires it)
	AC_PATH_PROG(GUILE,guile)
	if test "$GUILE" = "" ; then
	    AC_MSG_RESULT([Component mblock requires guile, which was not found.])
	    passed=no
	fi
        USRP_LIBUSB([],[passed=no;AC_MSG_RESULT([Unable to configure USB dependency.])])
        USRP_SET_FUSB_TECHNIQUE([],[passed=no;AC_MSG_RESULT([Unable to set fast USB technique.])])
        USRP_SDCC([2.4.0],[],[passed=no;AC_MSG_RESULT([Unable to find firmware compiler.])])

	usrp_INCLUDES="-I\${abs_top_srcdir}/usrp/host/lib/legacy \
		-I\${abs_top_srcdir}/usrp/firmware/include"
        usrp_LA="\${abs_top_builddir}/usrp/host/lib/legacy/libusrp.la"
	usrp_inband_INCLUDES="-I\${abs_top_srcdir}/usrp/host/lib/inband"
	usrp_inband_LA="\${abs_top_builddir}/usrp/host/lib/inband/libusrp_inband.la"
    fi

    GRC_BUILD_CONDITIONAL([usrp])

    AC_SUBST(usrp_INCLUDES)
    AC_SUBST(usrp_LA)
    AC_SUBST(usrp_inband_INCLUDES)
    AC_SUBST(usrp_inband_LA)
    AC_SUBST(usrp_darwin_omnithread_pc_requires)
    AC_SUBST(usrp_darwin_omnithread_pc_la)
])
