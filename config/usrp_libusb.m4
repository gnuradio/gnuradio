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
  LIBUSB_PKG_CONFIG_NAME=''
  if test x$1 = xyes; then
    PKG_CHECK_MODULES(USB, libusb-1.0, [
      libusbok=yes
      have_libusb1=yes
      usb_header='libusb-1.0/libusb.h'
      usb_lib_func='libusb_bulk_transfer'
      usb_lib_name='usb-1.0'
      LIBUSB_PKG_CONFIG_NAME='libusb-1.0'
    ])
  else
    dnl not using libusb1 (for now); see if legacy version is found.
    dnl it might be installed under the name either 'libusb' or
    dnl 'libusb-legacy', or just available via the
    dnl user's shell environment

    dnl see if the pkgconfig module 'libusb' is available
    PKG_CHECK_MODULES(USB, libusb, [
      libusbok=yes
      LIBUSB_PKG_CONFIG_NAME='libusb'
      ], [libusbok=no])
    dnl PKG_CHECK_MODULES does not work correctly when embedded
    if test $libusbok = no; then
      dnl if not, see if the pkgconfig module 'libusb-legacy' is available
      PKG_CHECK_MODULES(USB, [libusb-legacy], [
        libusbok=yes
        LIBUSB_PKG_CONFIG_NAME='libusb-legacy'
        ], [libusbok=no])
    fi
    dnl set variables for further testing
    usb_header='usb.h'
    usb_lib_func='usb_bulk_write'
    usb_lib_name='usb'
  fi
  AC_SUBST(LIBUSB_PKG_CONFIG_NAME)
  if test x$1 != xyes || test $have_libusb1 = yes; then
    dnl Either (1) libusb1 was specified and found; or
    dnl (2) libusb1 was not specified. Restart checking.
    libusbok=yes

    dnl Verify that $usb_header is a valid header, and if so,
    dnl then verify that $usb_lib_func can be found in the
    dnl library $usb_lib_name.

    dnl If PKGCONFIG found variable USB_INCLUDEDIR, and it is
    dnl not empty, use it for checking for $usb_header.
    dnl Otherwise, maybe the user's shell environment is already
    dnl configured to find this header.

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
      dnl On Darwin, need to include the IOKit library.     

      AC_LANG_PUSH(C)
      save_LIBS="$LIBS"
      LIBS=""
      case "$host_os" in
        darwin*)
          USB_LIBS="$USB_LIBS -lIOKit"
          LIBS="$USB_LIBS"
          ;;
        *) ;;
      esac
      AC_CHECK_LIB([$usb_lib_name], [$usb_lib_func], [], [
        libusbok=no
        AC_MSG_RESULT([USRP requires library '$usb_lib_name' with function '$usb_lib_func', which was either not found or was not usable. See http://www.libusb.org])
      ])
      case "$host_os" in
        cygwin* | mingw*)
          USB_LIBS="$LIBS"
          ;;
        *) ;;
      esac
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
