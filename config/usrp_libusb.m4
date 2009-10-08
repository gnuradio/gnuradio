dnl Copyright 2003,2008,2009 Free Software Foundation, Inc.
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

# $1 is $req_libusb1:
#   yes : check libusb-1.0
#   no  : check libusb-0.12
#   ""  : check libusb-0.12


AC_DEFUN([USRP_LIBUSB], [
  dnl Use PKGCONFIG to check for packages first, then check to
  dnl make sure the USB_* variables work (whether from PKGCONFIG
  dnl or overridden by the user)

  libusbok=no
  have_libusb1=no
  if test x$1 = xyes; then
    PKG_CHECK_MODULES(USB, libusb-1.0, [
      libusbok=yes
      have_libusb1=yes
      usb_header='libusb-1.0/libusb.h'
      usb_lib_func='libusb_bulk_transfer'
      usb_lib_name='usb-1.0'
    ])
  fi
  if test $libusbok = no; then
    dnl not using libusb1, or PKGCONFIG check for libusb1 failed;
    dnl see if legacy version is found.
    dnl it might be installed under the name either 'libusb' or
    dnl 'libusb-legacy'. check them in that order.

    dnl see if the pkgconfig module 'libusb' is available
    PKG_CHECK_MODULES(USB, libusb, [libusbok=yes], [libusbok=no])
    dnl PKG_CHECK_MODULES does not work correctly when embedded
    if test $libusbok = no; then
      dnl if not, see if the pkgconfig module 'libusb-legacy' is available
      PKG_CHECK_MODULES(USB, [libusb-legacy], [libusbok=yes], [libusbok=no])
    fi
    if test $libusbok = yes; then
      dnl if PKGCONFIG worked, set variables for further testing
      usb_header='usb.h'
      usb_lib_func='usb_bulk_write'
      usb_lib_name='usb'
    fi
  fi
  if test $libusbok = no; then
    AC_MSG_RESULT([USRP requires libusb, which was not found. See http://www.libusb.org])
  else

    dnl either some pkgconfig LIBUSB* variant was found, or USB_*
    dnl variables were overridden by the user as arguments to configure.
    dnl either way, check the USB_* variables to make sure they work

    AC_LANG_PUSH(C)
    save_CPPFLAGS="$CPPFLAGS"
    if test x$USB_INCLUDEDIR != x; then
      USB_INCLUDES="-I$USB_INCLUDEDIR"
      CPPFLAGS="$CPPFLAGS $USB_INCLUDES"
      AC_SUBST(USB_INCLUDES)
    fi
    AC_CHECK_HEADERS([$usb_header], [], [libusbok=no])
    CPPFLAGS="$save_CPPFLAGS"
    AC_LANG_POP(C)

    if test $libusbok = no; then
      AC_MSG_RESULT([USRP requires libusb header '$usb_header' which was not found or was not usable. See http://www.libusb.org])
    else

      dnl found the header; now make sure the library is OK

      AC_LANG_PUSH(C)
      save_LIBS="$LIBS"
      case "$host_os" in
        darwin*)
          USB_LIBS="$USB_LIBS -lIOKit"
          ;;
        *) ;;
      esac
      LIBS="$LIBS $USB_LIBS"
      AC_CHECK_LIB([$usb_lib_name], [$usb_lib_func], [], [
        libusbok=no
        AC_MSG_RESULT([USRP requires library '$usb_lib_name' with function '$usb_lib_func', which was either not found or was not usable. See http://www.libusb.org])
      ])
      LIBS="$save_LIBS"
      AC_LANG_POP(C)
    fi
  fi
  if test $libusbok = yes; then
    AC_SUBST(USB_LIBS)
    ifelse([$2], , :, [$2])
  else
    USB_INCLUDES=
    USB_LIBS=
    ifelse([$3], , :, [$3])
  fi
])
