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

  dnl do not use LDFLAGS, since PKGCONFIG will provide everything
  saved_LDFLAGS=${LDFLAGS}
  LDFLAGS=
  LIBUSB_PKG_CONFIG_NAME=

  dnl loop over various possible 'libusb' PKGCONFIG names, and choose
  dnl the first one that meets both the user's selection (via
  dnl configure flags) as well as what is installed

  dnl create the list of libusb PKGCONFIG modules to test
  libusb_list=''
  if test x$1 = xyes; then
    dnl libusb-1.0 was requested; just test for it
    libusb_list="libusb-1.0"
  else
    dnl test for legacy libusb only
    libusb_list="libusb libusb-legacy"
  fi
  for libusb_name in ${libusb_list}; do
    dnl clear internal variables
    libusbok=no
    have_libusb1=no
    usb_header=''
    usb_lib_func=''
    usb_lib_name=''

    dnl start checks
    AC_MSG_NOTICE([Checking for LIBUSB version '${libusb_name}'])
    if test ${libusb_name} = "libusb-1.0"; then
      dnl see if the pkgconfig module is available
      PKG_CHECK_MODULES(USB, ${libusb_name}, [
        libusbok=yes
        have_libusb1=yes
        usb_header='libusb-1.0/libusb.h'
        usb_lib_func='libusb_bulk_transfer'
      ], [libusbok=no])
    else
      dnl see if the pkgconfig module is available
      PKG_CHECK_MODULES(USB, ${libusb_name}, [
        libusbok=yes
        usb_header='usb.h'
        usb_lib_func='usb_bulk_write'
      ], [libusbok=no])
    fi
    if test $libusbok = yes; then
      dnl PKGCONFIG found a version of LIBUSB.
      dnl Check it to make sure it meets enough criteria:
      dnl Verify that $usb_header is a valid header. If so, then
      dnl verify that $usb_lib_func can be found in the library
      dnl $usb_lib_name.  if so, verify that the symbol 'usb_debug' is
      dnl found in the library.

      dnl Check for the header.  Similar to AC_CHECK_HEADERS,
      dnl but doesn't append to known \#defines.
      dnl If PKGCONFIG found variable USB_INCLUDEDIR, and it is
      dnl not empty, use it for checking for $usb_header.
      dnl Otherwise, maybe the user's shell environment is already
      dnl configured to find this header.
      AC_LANG_PUSH(C)
      save_CPPFLAGS="$CPPFLAGS"
      if test x$USB_INCLUDEDIR != x; then
        USB_INCLUDES="-I$USB_INCLUDEDIR"
        CPPFLAGS="$USB_INCLUDES"
      fi
      AC_MSG_CHECKING([$libusb_name for header $usb_header])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
        #include "$usb_header"
        ]], [])],
        [libusbok=yes],[libusbok=no])
      AC_MSG_RESULT([$libusbok])
      CPPFLAGS="$save_CPPFLAGS"
      AC_LANG_POP(C)

      if test $libusbok = yes; then
        dnl found the header; now make sure the library is OK
        dnl On Darwin, need to include the IOKit library.     

        AC_LANG_PUSH(C)
        save_LIBS="$LIBS"
        case "$host_os" in
          darwin*)
            USB_LIBS="$USB_LIBS -lIOKit"
            ;;
          *) ;;
        esac
        LIBS="$USB_LIBS"

 	dnl find the library link name
        usb_lib_name=`echo $USB_LIBS | sed -e "s@.*-l\(usb[[^ ]]*\).*@\1@"`

        dnl Check for the function in the library.  Similar to
        dnl AC_CHECK_LIB, but doesn't append to known \#defines.
        AC_MSG_CHECKING([$libusb_name for function $usb_lib_func in library $usb_lib_name])
        AC_LINK_IFELSE([AC_LANG_PROGRAM([[
          #ifdef __cplusplus
          extern "C"
          #endif
          char $usb_lib_func ();
          ]], [[return $usb_lib_func ();]])],
          [libusbok=yes],[libusbok=no])
        AC_MSG_RESULT([$libusbok])
        case "$host_os" in
          cygwin* | mingw*)
            USB_LIBS="$LIBS"
            ;;
          *) ;;
        esac
        LIBS="$save_LIBS"
        AC_LANG_POP(C)

        if test $libusbok = yes; then
          if test ${libusb_name} != "libusb-1.0"; then
            dnl PKGCONFIG found a legacy version of libusb; make sure the
            dnl variable _usb_debug is available in the found library
            AC_LANG_PUSH(C)
            save_CPPFLAGS="$CPPFLAGS"
            CPPFLAGS="$USB_INCLUDES"
            save_LIBS="$LIBS"
            LIBS="$USB_LIBS"
            AC_MSG_CHECKING([$libusb_name for symbol usb_debug in library $usb_lib_name])
            AC_LINK_IFELSE([AC_LANG_PROGRAM([[
              extern int usb_debug;]],
              [[usb_debug = 0;]])],
              [libusbok=yes],[libusbok=no])
            AC_MSG_RESULT([$libusbok])
            LIBS="$save_LIBS"
            CPPFLAGS="$save_CPPFLAGS"
            AC_LANG_POP(C)
          fi
        fi
      fi
    fi
    dnl if everything checks out OK, finish up
    if test $libusbok = yes; then
      LIBUSB_PKG_CONFIG_NAME="${libusb_name}"
      break
    else
      dnl something wasn't found in this LIBUSB version.
      dnl HACK: clear cache variables for header and library
      unset USB_INCLUDEDIR
      unset USB_INCLUDES
      unset USB_LIBS
      unset $as_ac_Header
      unset $as_ac_Lib
    fi
  done

  if test $libusbok = yes; then
    dnl final error checking, mostly to create #define's
    AC_LANG_PUSH(C)
    save_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$USB_INCLUDES"
    dnl Check for the header.
    AC_CHECK_HEADERS([$usb_header], [], [libusbok=no])
    CPPFLAGS="$save_CPPFLAGS"
    AC_LANG_POP(C)

    if test $libusbok = no; then
      AC_MSG_RESULT([USRP requires libusb header '$usb_header' which was not found or was not usable. See http://www.libusb.org])
    else
      dnl check for the library (again)
      AC_LANG_PUSH(C)
      save_CPPFLAGS="$CPPFLAGS"
      CPPFLAGS="$USB_INCLUDES"
      save_LIBS="$LIBS"
      LIBS="$USB_LIBS"
      AC_CHECK_LIB([$usb_lib_name], [$usb_lib_func], [], [
        libusbok=no
        AC_MSG_RESULT([USRP requires library '$usb_lib_name' with function '$usb_lib_func', which was either not found or was not usable. See http://www.libusb.org])])
      LIBS="$save_LIBS"
      CPPFLAGS="$save_CPPFLAGS"
      AC_LANG_POP(C)
    fi
  fi

  if test $libusbok = yes; then
    dnl success
    AC_MSG_NOTICE([Using LIBUSB version '${libusb_name}'])
    ifelse([$2], , :, [$2])
  else
    dnl not found; clear substitution variables
    LIBUSB_PKG_CONFIG_NAME=
    USB_INCLUDES=
    USB_LIBS=
    ifelse([$3], , :, [$3])
  fi

  dnl create substitution variables
  AC_SUBST(USB_INCLUDES)
  AC_SUBST(USB_LIBS)
  AC_SUBST(LIBUSB_PKG_CONFIG_NAME)

  dnl restore LDFLAGS
  LDFLAGS=${saved_LDFLAGS}
])
