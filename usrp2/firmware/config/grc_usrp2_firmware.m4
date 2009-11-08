dnl Copyright 2008 Free Software Foundation, Inc.
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

dnl Fix 2.64 cross compile detection for AVR and RTEMS
dnl by not trying to compile fopen.
m4_if(m4_defn([m4_PACKAGE_VERSION]), [2.64],
  [m4_foreach([_GCC_LANG], [C, C++, Fortran, Fortran 77],
     [m4_define([_AC_LANG_IO_PROGRAM(]_GCC_LANG[)], m4_defn([AC_LANG_PROGRAM(]_GCC_LANG[)]))])]) 

AC_DEFUN([GRC_USRP2_FIRMWARE],[
    dnl we use  --enable-usrp2-firmware to enable this
    GRC_ENABLE(usrp2-firmware)

    GRC_CHECK_DEPENDENCY(usrp2-firmware, usrp2)

    AC_REQUIRE([AC_PROG_CC])
    AC_REQUIRE([AC_PROG_CPP])
    AC_REQUIRE([AM_PROG_AS])
    AC_REQUIRE([AC_PROG_RANLIB])


    AC_CHECK_HEADERS(arpa/inet.h netinet/in.h byteswap.h)
    AC_C_BIGENDIAN

    dnl If execution gets to here, $passed will be:
    dnl   with : if the --with code didn't error out
    dnl   yes  : if the --enable code passed muster and all dependencies are met
    dnl   no   : otherwise
    if test $passed = yes; then

	dnl Only do firmware if mb-gcc can be found
	AC_CHECK_PROG([MB_GCC],[mb-gcc],[yes],[no])
	if test $MB_GCC = no; then
	    AC_MSG_RESULT([usrp2 firmware requires mb-gcc.  Not found])
	    passed=no
	fi
    fi
    if test $passed != with; then
	dnl how and where to find INCLUDES and LA
	dnl USRP2_INCLUDES="-I\${abs_top_srcdir}/usrp2/host/include \
	dnl	-I\${abs_top_srcdir}/usrp2/firmware/include"
        dnl USRP2_LA="\${abs_top_builddir}/usrp2/host/lib/libusrp2.la"
	:
    fi

    dnl Include the usrp2 INCLUDES and LA
    dnl AC_SUBST(USRP2_INCLUDES)
    dnl AC_SUBST(USRP2_LA)

    AC_CONFIG_FILES([ \
        apps/Makefile \
        include/Makefile \
        lib/Makefile \
    ])

    dnl Slightly non-standard:  we handle this with an AM_CONDITIONAL
    AM_CONDITIONAL(BUILDING_USRP2_FIRMWARE, [test $passed = yes && test "$enable_usrp2_firmware" != no])

    GRC_BUILD_CONDITIONAL(usrp2-firmware)
])
