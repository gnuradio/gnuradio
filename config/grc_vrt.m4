dnl Copyright 2008,2009 Free Software Foundation, Inc.
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

AC_DEFUN([GRC_VRT],[
    GRC_ENABLE(vrt)

    dnl If execution gets to here, $passed will be:
    dnl   with : if the --with code didn't error out
    dnl   yes  : if the --enable code passed muster and all dependencies are met
    dnl   no   : otherwise
    if test $passed = yes; then
	dnl Needed for vrt_socket_opener
	AC_CHECK_HEADERS(arpa/inet.h byteswap.h linux/if_packet.h sys/socket.h sys/un.h)
    fi
    if test $passed != with; then
	dnl how and where to find INCLUDES and LA
	VRT_INCLUDES="-I\${abs_top_srcdir}/vrt/include"
        VRT_LA="\${abs_top_builddir}/vrt/lib/libvrt.la"
    fi

    dnl Include the vrt INCLUDES and LA
    AC_SUBST(VRT_INCLUDES)
    AC_SUBST(VRT_LA)

    AC_CONFIG_FILES([
        vrt/Makefile
        vrt/vrt.pc
        vrt/include/Makefile
        vrt/include/vrt/Makefile
        vrt/lib/Makefile
        vrt/apps/Makefile
    ])

    GRC_BUILD_CONDITIONAL(vrt)
])
