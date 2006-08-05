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
dnl the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
dnl Boston, MA 02111-1307, USA.

AC_DEFUN([GRC_USRP],[
    AC_CONFIG_SRCDIR([usrp/host/lib/usrp_prims.h])

    AC_CONFIG_FILES([ \
	usrp/Makefile				\
	usrp/usrp.pc 				\
	usrp/usrp.iss				\
	usrp/doc/Doxyfile			\
	usrp/doc/Makefile			\
	usrp/doc/other/Makefile			\
	usrp/host/Makefile			\
	usrp/host/misc/Makefile			\
	usrp/host/lib/Makefile			\
	usrp/host/lib/std_paths.h		\
	usrp/host/swig/Makefile			\
	usrp/host/apps/Makefile			\
	usrp/firmware/Makefile			\
	usrp/firmware/include/Makefile		\
	usrp/firmware/lib/Makefile		\
	usrp/firmware/src/Makefile 		\
	usrp/firmware/src/common/Makefile 	\
	usrp/firmware/src/usrp2/Makefile 	\
	usrp/fpga/Makefile			\
	usrp/fpga/rbf/Makefile			\
    ])

    AC_CHECK_PROG([XMLTO],[xmlto],[yes],[])
    AM_CONDITIONAL(HAS_XMLTO, test x$XMLTO = xyes)

    # gnulib.
    # FIXME: this needs to fail gracefully and continue, not implemented yet
    UTILS_FUNC_MKSTEMP

    succeeded=yes
    dnl Checks for library functions.
    USRP_LIBUSB([],[succeeded=no])
    USRP_SET_FUSB_TECHNIQUE([],[succeeded=no])

    dnl check for firmware cross compilation tools
    dnl check for SDCC 2.4.0
    USRP_SDCC([2.4.0],[],[succeeded=no])
    
    AC_CHECK_FUNCS([getrusage sched_setscheduler],[],[succeeded=no])
    AC_CHECK_FUNCS([sigaction snprintf],[],[succeeded=no])

    dnl we use these to handle possible byteswapping to and from the USRP.
    AC_CHECK_HEADERS([byteswap.h],[],[succeeeded=no])
    AC_C_BIGENDIAN

    if test $succeeded = yes; then
	dnl Define where to look for USRP includes
	USRP_INCLUDES='-I$(top_srcdir)/host/lib -I$(top_srcdir)/firmware/include'
	AC_SUBST(USRP_INCLUDES)

        USRP_DEFINES=''
        AC_SUBST(USRP_DEFINES)

        DEFINES="$USRP_DEFINES"
        AC_SUBST(DEFINES)

	subdirs="$subdirs usrp"
    else
	failed="$failed usrp"
    fi
])
