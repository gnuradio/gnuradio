dnl
dnl Copyright 2010,2011 Free Software Foundation, Inc.
dnl
dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.
dnl

AC_INIT
AC_PREREQ(2.57)
AC_CONFIG_AUX_DIR([.])	
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(volk,0.1)

dnl This is kind of non-standard, but it sure shortens up this file :-)

m4_include([config/lv_configure.m4])

LV_CONFIGURE

dnl Check for any libraries you need
dnl AC_CHECK_LIBRARY

dnl Check for header files you need
dnl AC_CHECK_HEADERS(fcntl.h limits.h strings.h sys/ioctl.h sys/time.h unistd.h)
dnl AC_CHECK_HEADERS(sys/mman.h)

dnl Checks for library functions.
dnl AC_CHECK_FUNCS([])

dnl We pick up the boost cppflags, cxxflags and thread lib via GNURADIO_CORE
dnl
dnl If you need additional boost libraries, you'll need to
dnl uncomment AX_BOOST_BASE, plus some of the following:
dnl
dnl calls AC_SUBST(BOOST_CPPFLAGS), AC_SUBST(BOOST_LDFLAGS) and defines HAVE_BOOST
AX_BOOST_BASE([1.35])
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
AX_BOOST_UNIT_TEST_FRAMEWORK
dnl AX_BOOST_WSERIALIZATION

LV_SET_SIMD_FLAGS

AC_CONFIG_FILES([\
	  Makefile \
	  config/Makefile \
	  include/Makefile \
	  include/volk/Makefile \
	  lib/Makefile \
	  orc/Makefile \
	  volk.pc \
	])

AC_OUTPUT

echo ""
echo "The following architectures will be built:"
echo "    $BUILT_ARCHS"
echo ""

