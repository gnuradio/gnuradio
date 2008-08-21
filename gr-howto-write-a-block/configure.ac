dnl 
dnl  Copyright 2004,2005,2007,2008 Free Software Foundation, Inc.
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

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(gr-howto-write-a-block,3.1.svn)

LF_CONFIGURE_CC
LF_CONFIGURE_CXX
GR_LIB64		dnl check for lib64 suffix after choosing compilers


dnl add ${prefix}/lib${gr_libdir_suffix}/pkgconfig to the head of the PKG_CONFIG_PATH
if test x${PKG_CONFIG_PATH} = x; then
    PKG_CONFIG_PATH=${prefix}/lib${gr_libdir_suffix}/pkgconfig
else
    PKG_CONFIG_PATH=${prefix}/lib${gr_libdir_suffix}/pkgconfig:${PKG_CONFIG_PATH}
fi
export PKG_CONFIG_PATH

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
m4_ifdef([LT_INIT],[LT_INIT],[AC_PROG_LIBTOOL])
dnl GR_FORTRAN

GR_NO_UNDEFINED		dnl do we need the -no-undefined linker flag
GR_SCRIPTING		dnl Locate python, SWIG, etc

AC_CHECK_PROG([XMLTO],[xmlto],[yes],[])
AM_CONDITIONAL([HAS_XMLTO], [test x$XMLTO = xyes])


dnl Checks for libraries.

dnl Set the c++ compiler that we use for the build system when cross compiling
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
AC_C_BIGENDIAN


dnl Checks for library functions.
AC_CHECK_FUNCS([])

dnl Check for Mingw support
GR_PWIN32

PKG_CHECK_MODULES(GNURADIO_CORE, gnuradio-core >= 3)
LIBS="$LIBS $GNURADIO_CORE_LIBS"

dnl We pick up the boost cppflags, cxxflags and thread lib via GNURADIO_CORE
dnl
dnl If you need additional boost libraries, you'll need to
dnl uncomment AX_BOOST_BASE, plus some of the following:
dnl
dnl calls AC_SUBST(BOOST_CPPFLAGS), AC_SUBST(BOOST_LDFLAGS) and defines HAVE_BOOST
dnl AX_BOOST_BASE([1.35])
dnl
dnl All the rest of these call AC_SUBST(BOOST_<foo>_LIB) and define HAVE_BOOST_<foo>
dnl
dnl AX_BOOST_DATE_TIME
dnl AX_BOOST_FILESYSTEM
dnl AX_BOOST_IOSTREAMS
dnl AX_BOOST_PROGRAM_OPTIONS
dnl AX_BOOST_REGEX
dnl AX_BOOST_SERIALIZATION
dnl AX_BOOST_SIGNALS
dnl AX_BOOST_SYSTEM
dnl AX_BOOST_TEST_EXEC_MONITOR
dnl AX_BOOST_UNIT_TEST_FRAMEWORK
dnl AX_BOOST_WSERIALIZATION


STD_DEFINES_AND_INCLUDES="$GNURADIO_CORE_CFLAGS"
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
