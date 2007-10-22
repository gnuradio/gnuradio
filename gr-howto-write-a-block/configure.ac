dnl 
dnl  Copyright 2004,2005,2007 Free Software Foundation, Inc.
dnl  
dnl  This file is part of GNU Radio
dnl  
dnl  GNU Radio is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU General Public License as published by
dnl  the Free Software Foundation; either version 3, or (at your option)
dnl  any later version.
dnl  
dnl  GNU Radio is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl  GNU General Public License for more details.
dnl  
dnl  You should have received a copy of the GNU General Public License
dnl  along with GNU Radio; see the file COPYING.  If not, write to
dnl  the Free Software Foundation, Inc., 51 Franklin Street,
dnl  Boston, MA 02110-1301, USA.
dnl 

AC_INIT
AC_PREREQ(2.57)
AC_CONFIG_SRCDIR([src/lib/howto.i])
AM_CONFIG_HEADER(config.h)
AC_CANONICAL_TARGET([])
AM_INIT_AUTOMAKE(gr-howto-write-a-block,3.1.0rc0)

GR_X86_64
dnl LF_CONFIGURE_CC
LF_CONFIGURE_CXX
LF_SET_WARNINGS
GR_SET_GPROF
GR_SET_PROF
AM_PROG_AS
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PATH_PROG([RM_PROG], [rm])

AC_LIBTOOL_WIN32_DLL
AC_ENABLE_SHARED	dnl do build shared libraries
AC_DISABLE_STATIC	dnl don't build static libraries
AC_PROG_LIBTOOL

dnl Locate python, SWIG, etc
GR_NO_UNDEFINED
GR_SCRIPTING

dnl Checks for libraries.

dnl check for threads (mandatory)
GR_OMNITHREAD

CFLAGS="${CFLAGS} $PTHREAD_CFLAGS"
CXXFLAGS="${CXXFLAGS} $PTHREAD_CFLAGS"

if test "x$CXX_FOR_BUILD" = x
then
  CXX_FOR_BUILD=${CXX}
fi
AC_SUBST(CXX_FOR_BUILD)

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(fcntl.h limits.h strings.h sys/ioctl.h sys/time.h unistd.h)
AC_CHECK_HEADERS(sys/mman.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME

dnl Checks for library functions.
AC_CHECK_FUNCS([])

dnl Check for Mingw support
GR_PWIN32

PKG_CHECK_MODULES(GNURADIO_CORE, gnuradio-core >= 2)
LIBS="$LIBS $GNURADIO_CORE_LIBS"

dnl Define where to find boost includes
dnl defines BOOST_CFLAGS
GR_REQUIRE_BOOST_INCLUDES

STD_DEFINES_AND_INCLUDES="$GNURADIO_CORE_CFLAGS $BOOST_CFLAGS"
AC_SUBST(STD_DEFINES_AND_INCLUDES)
 
AC_CONFIG_FILES([\
	  Makefile \
	  config/Makefile \
	  doc/Makefile \
	  src/Makefile \
	  src/lib/Makefile \
	  src/python/Makefile \
	  src/python/run_tests \
	])

dnl run_tests is created from run_tests.in.  Make it executable.
AC_CONFIG_COMMANDS([run_tests], [chmod +x src/python/run_tests])

AC_OUTPUT
