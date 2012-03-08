dnl Copyright 2003,2008 Free Software Foundation, Inc.
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

AC_DEFUN([USRP_LIBUSB], [
    libusbok=yes
    PKG_CHECK_MODULES(USB, libusb, [], [
        AC_LANG_PUSH(C)

	AC_CHECK_HEADERS([usb.h], [], [libusbok=no; AC_MSG_RESULT([USRP requires libusb. usb.h not found. See http://libusb.sf.net])])

	save_LIBS="$LIBS"
	case "$host_os" in
	  darwin*)
	    LIBS="$LIBS -lIOKit"
            ;;
	  *) ;;
        esac

	AC_SEARCH_LIBS(usb_bulk_write, [usb], [USB_LIBS="$LIBS"], [libusbok=no; AC_MSG_RESULT([USRP requires libusb. usb_bulk_write not found. See http://libusb.sf.net])])

        LIBS="$save_LIBS"

        AC_LANG_POP
    ])

    if test x$libusbok = xyes; then
        AC_SUBST(USB_LIBS)
	ifelse([$1], , :, [$1])
    else
        ifelse([$2], , :, [$2])
    fi
])
