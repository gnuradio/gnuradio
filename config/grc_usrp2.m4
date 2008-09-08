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

AC_DEFUN([GRC_USRP2],[
    GRC_ENABLE(usrp2)

    dnl Don't do usrp if omnithread or gruel is skipped
    GRC_CHECK_DEPENDENCY(usrp2, gruel)
    GRC_CHECK_DEPENDENCY(usrp2, omnithread)

    dnl USRP2 host code only works on Linux at the moment
    AC_MSG_CHECKING([whether host_os is linux*])
    case "$host_os" in
      linux*)
	AC_MSG_RESULT([yes])
        ;;
      *)
	AC_MSG_RESULT([no])
	AC_MSG_NOTICE([USRP2 currently requires Linux host OS, not found])
        passed="no"
        ;;
    esac

    dnl If execution gets to here, $passed will be:
    dnl   with : if the --with code didn't error out
    dnl   yes  : if the --enable code passed muster and all dependencies are met
    dnl   no   : otherwise
    if test $passed = yes; then
	dnl Only do firmware if mb-gcc can be found
	AC_CHECK_PROG([MB_GCC],[mb-gcc],[yes],[no])
	if test $MB_GCC = yes; then
	    dnl Adds usrp2/firmware to $(subdirs), hierarchical build
	    AC_CONFIG_SUBDIRS([usrp2/firmware])
	else
	    AC_MSG_WARN([usrp2/firmware is not being built])
	fi
	AM_CONDITIONAL([BUILDING_USRP2_FIRMWARE],[test $MB_GCC = yes])

	dnl Needed for usrp2_socket_opener
	AC_CHECK_HEADERS(arpa/inet.h byteswap.h linux/if_packet.h sys/socket.h sys/un.h)
	AC_CHECK_MEMBERS([struct msghdr.msg_control,
			  struct msghdr.msg_accrights,
			  struct cmsgcred.cmcred_uid,
			  struct ucred.uid],
        	         [],[],
			 [#define __USE_GNU 1
			  #include <sys/types.h>
		          #include <sys/socket.h>
			  #include <sys/un.h>])
    fi
    if test $passed != with; then
	dnl how and where to find INCLUDES and LA
	USRP2_INCLUDES="-I\${abs_top_srcdir}/usrp2/host/include \
		-I\${abs_top_srcdir}/usrp2/firmware/include"
        USRP2_LA="\${abs_top_builddir}/usrp2/host/lib/libusrp2.la"
    fi

    dnl Include the usrp2 INCLUDES and LA
    AC_SUBST(USRP2_INCLUDES)
    AC_SUBST(USRP2_LA)

    AC_CONFIG_FILES([ \
        usrp2/Makefile
        usrp2/host/Makefile \
        usrp2/host/usrp2.pc \
        usrp2/host/include/Makefile
        usrp2/host/include/usrp2/Makefile
        usrp2/host/lib/Makefile
        usrp2/host/apps/Makefile
    ])

    GRC_BUILD_CONDITIONAL(usrp2)
])
