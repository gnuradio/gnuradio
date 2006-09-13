# Check for libusb support.             		-*- Autoconf -*-

# Copyright 2003 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Boston, MA
# 02110-1301, USA.

AC_DEFUN([USRP_LIBUSB],
[
	AC_LANG_PUSH(C)

	libusbok=yes
	AC_CHECK_HEADERS([usb.h],
	  [],
	  [libusbok=no, 
	   AC_MSG_RESULT([USRP requires libusb. usb.h not found. See http://libusb.sf.net])]
	)

        save_LIBS="$LIBS"
	case "$host_os" in
	    darwin*)    LIBS="$LIBS -lIOKit" ;;
	    *) ;;
	esac
	AC_SEARCH_LIBS(usb_bulk_write, [usb],
	  [USB_LIBS="$LIBS"],
	  [libusbok=no,
	   AC_MSG_RESULT([USRP requires libusb. usb_bulk_write not found. See http://libusb.sf.net])]
	)
        LIBS="$save_LIBS"

	AC_LANG_POP

	if test $libusbok = yes; then
	    AC_SUBST(USB_LIBS)
	    ifelse([$1], , :, [$1])
	else
	    ifelse([$2], , :, [$2])
	fi
])
