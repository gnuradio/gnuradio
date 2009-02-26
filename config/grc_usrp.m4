dnl Copyright 2001,2002,2003,2004,2005,2006,2007,2008,2009 Free Software Foundation, Inc.
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
    GRC_ENABLE(usrp)

    GRC_WITH(usrp, [GRC_WITH_PKG_CONFIG_CHECK(usrp-inband)])

    dnl Don't do usrp if omnithread, mblock, or pmt skipped
    GRC_CHECK_DEPENDENCY(usrp, omnithread)
    GRC_CHECK_DEPENDENCY(usrp, mblock)
    GRC_CHECK_DEPENDENCY(usrp, pmt)

    dnl Make sure the fast usb technique is set, OS dependent.
    dnl This is always performed, since it puts out CLI flags.
    USRP_SET_FUSB_TECHNIQUE(${enable_usrp})

    dnl If execution gets to here, $passed will be:
    dnl   with : if the --with code didn't error out
    dnl   yes  : if the --enable code passed muster and all dependencies are met
    dnl   no   : otherwise
    if test $passed = yes; then
        dnl gnulib.
        dnl FIXME: this needs to fail gracefully and continue, not implemented yet
        UTILS_FUNC_MKSTEMP

        dnl These checks don't fail
        AC_C_BIGENDIAN
        AC_CHECK_HEADERS([byteswap.h linux/compiler.h])
        AC_CHECK_FUNCS([getrusage sched_setscheduler pthread_setschedparam])
        AC_CHECK_FUNCS([sigaction snprintf])

	dnl Don't do usrp if guile not available (inband requires it)
	GRC_CHECK_GUILE(usrp)

	dnl Make sure libusb is installed; required for legacy USB
        USRP_LIBUSB([],[passed=no;AC_MSG_RESULT([Unable to find dependency libusb.])])

	dnl Make sure SDCC >= 2.4.0 is available.
        USRP_SDCC([2.4.0],[],[passed=no;AC_MSG_RESULT([Unable to find firmware compiler SDCC.])])
    fi
    if test $passed != with; then
	dnl how and where to find INCLUDES and LA
	usrp_INCLUDES="-I\${abs_top_srcdir}/usrp/host/lib/legacy \
		-I\${abs_top_srcdir}/usrp/firmware/include \
		-I\${abs_top_builddir}/usrp/host/lib/legacy"
        usrp_LA="\${abs_top_builddir}/usrp/host/lib/legacy/libusrp.la"
	usrp_inband_INCLUDES="-I\${abs_top_srcdir}/usrp/host/lib/inband"
	usrp_inband_LA="\${abs_top_builddir}/usrp/host/lib/inband/libusrp-inband.la"
    fi

    dnl Include the usrp-inband INCLUDES and LA
    AC_SUBST(usrp_inband_INCLUDES)
    AC_SUBST(usrp_inband_LA)

    dnl There are 2 pkg-config files (usrp, and usrp-inband); the one
    dnl for usrp requires omnithread for Darwin only.  Create a variable
    dnl for just the usrp.pc.in case.
    case "$host_os" in
      darwin*)
        usrp_darwin_omnithread_pc_requires="gnuradio-omnithread"
        ;;
      *) dnl (blanks)
        usrp_darwin_omnithread_pc_requires=""
        ;;
    esac
    AC_SUBST(usrp_darwin_omnithread_pc_requires)

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
        usrp/fpga/rbf/rev2/Makefile \
        usrp/fpga/rbf/rev4/Makefile \
    ])

    GRC_BUILD_CONDITIONAL(usrp)
])
