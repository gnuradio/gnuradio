dnl
dnl Copyright 2003 Free Software Foundation, Inc.
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
dnl the Free Software Foundation, Inc., 51 Franklin Street,
dnl Boston, MA 02110-1301, USA.
dnl 

AC_DEFUN([USRP_SET_FUSB_TECHNIQUE],[
  AC_ARG_WITH(fusb-tech,
	[  --with-fusb-tech=OS     set fast usb technique (auto)],
		[cf_with_fusb_tech="$withval"],
		[cf_with_fusb_tech="$host_os"])
	

  AC_CHECK_HEADER([linux/usbdevice_fs.h],
	[x_have_usbdevice_fs_h=yes],
	[x_have_usbdevice_fs_h=no])

  AC_MSG_CHECKING([for fast usb technique to use])
  case "$cf_with_fusb_tech" in
    linux*)	if test x${x_have_usbdevice_fs_h} = xyes;
		then
		  FUSB_TECH=linux
                else
		  FUSB_TECH=generic
		fi 			;;

    darwin*)	FUSB_TECH=darwin 	;;
    cygwin*|win*|mingw*)	FUSB_TECH=win32		;;
    *bsd*)
		AC_MSG_CHECKING([for RA/WB])
		AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <dev/usb/usb.h>]],
						   [[struct usb_bulk_ra_wb_opt o;
						     ioctl(0, USB_SET_BULK_RA, &o);]])],
				  [FUSB_TECH=ra_wb],
				  [FUSB_TECH=generic])		;;
    *)		FUSB_TECH=generic	;;
  esac	

  AC_MSG_RESULT($FUSB_TECH)
  AC_SUBST(FUSB_TECH)

  AM_CONDITIONAL(FUSB_TECH_darwin,   test $FUSB_TECH = darwin)
  AM_CONDITIONAL(FUSB_TECH_win32,    test $FUSB_TECH = win32)
  AM_CONDITIONAL(FUSB_TECH_generic,  test $FUSB_TECH = generic)
  AM_CONDITIONAL(FUSB_TECH_linux,    test $FUSB_TECH = linux)
  AM_CONDITIONAL(FUSB_TECH_ra_wb,    test $FUSB_TECH = ra_wb)
])

