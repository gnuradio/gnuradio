dnl Copyright 2001,2002,2003,2004,2005,2006 Free Software Foundation, Inc.
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

    AC_CONFIG_FILES([ \
	usrp/Makefile \
	usrp/usrp.pc \
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
    AC_CHECK_FUNCS([getrusage sched_setscheduler])
    AC_CHECK_FUNCS([sigaction snprintf])

    passed=yes
    # Don't do usrp if mblock skipped
    # There *has* to be a better way to check if a value is in a string
    for dir in $skipped_dirs
    do
	if test "$dir" = "mblock"; then
	    AC_MSG_RESULT([Component usrp requires mblock, which is not being built.])
	    passed=no
	fi
    done

    USRP_LIBUSB([],[passed=no;AC_MSG_RESULT([Unable to configure USB dependency.])])
    USRP_SET_FUSB_TECHNIQUE([],[passed=no;AC_MSG_RESULT([Unable to set fast USB technique.])])
    USRP_SDCC([2.4.0],[],[passed=no;AC_MSG_RESULT([Unable to find firmware compiler.])])

    GRC_BUILD_CONDITIONAL([usrp])
])
