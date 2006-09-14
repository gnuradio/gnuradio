dnl Copyright 2001,2002,2003,2004,2005,2006 Free Software Foundation, Inc.
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

AC_INIT
AC_PREREQ(2.57)
AM_CONFIG_HEADER(config.h)
AC_CANONICAL_HOST
AC_CANONICAL_TARGET([])
AM_INIT_AUTOMAKE(gnuradio,2.8svn)
AC_CONFIG_SRCDIR([gnuradio-core/src/lib/runtime/gr_vmcircbuf.cc])

GR_X86_64		dnl check for lib64 suffix
LF_CONFIGURE_CC
LF_CONFIGURE_CXX
LF_SET_WARNINGS
GR_SET_GPROF
GR_SET_PROF
GR_PROG_AS
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PATH_PROG([RM_PROG], [rm])

AC_LIBTOOL_WIN32_DLL
dnl AC_DISABLE_SHARED	dnl don't build shared libraries
AC_ENABLE_SHARED	dnl do build shared libraries
AC_DISABLE_STATIC	dnl don't build static libraries
AC_PROG_LIBTOOL
GR_FORTRAN

GR_NO_UNDEFINED		dnl do we need the -no-undefined linker flag
GR_SCRIPTING

AC_CHECK_PROG([XMLTO],[xmlto],[yes],[])
AM_CONDITIONAL([HAS_XMLTO], [test x$XMLTO = xyes])

dnl Checks for libraries.
AC_CHECK_LIB(socket,socket)

dnl check for threads (mandatory)
GR_OMNITHREAD
#AC_SUBST(PTHREAD_CFLAGS)
CFLAGS="${CFLAGS} $PTHREAD_CFLAGS"
CXXFLAGS="${CXXFLAGS} $PTHREAD_CFLAGS"

if test "x$CXX_FOR_BUILD" = x
then
  CXX_FOR_BUILD=${CXX}
fi
AC_SUBST(CXX_FOR_BUILD)

dnl Check for SysV shm (mandatory)
dnl FIXME this is no longer mandatory.  Check the macro.
GR_SYSV_SHM

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(fcntl.h limits.h strings.h time.h sys/ioctl.h sys/time.h unistd.h)
AC_CHECK_HEADERS(linux/ppdev.h sys/mman.h sys/select.h sys/types.h)
AC_CHECK_HEADERS(sys/resource.h stdint.h sched.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_BIGENDIAN
AC_STRUCT_TM

dnl Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([mmap select socket strcspn strerror strspn getpagesize sysconf])
AC_CHECK_FUNCS([snprintf gettimeofday nanosleep sched_setscheduler])
AC_CHECK_FUNCS([modf sqrt])

AC_CHECK_LIB(m, sincos, [AC_DEFINE([HAVE_SINCOS],[1],[Define to 1 if your system has `sincos'.])])
AC_CHECK_LIB(m, sincosf,[AC_DEFINE([HAVE_SINCOSF],[1],[Define to 1 if your system has `sincosf'.])])
AC_CHECK_LIB(m, sinf, [AC_DEFINE([HAVE_SINF],[1],[Define to 1 if your system has `sinf'.])])
AC_CHECK_LIB(m, cosf, [AC_DEFINE([HAVE_COSF],[1],[Define to 1 if your system has `cosf'.])])
AC_CHECK_LIB(m, trunc, [AC_DEFINE([HAVE_TRUNC],[1],[Define to 1 if your system has `trunc'.])])
AC_CHECK_LIB(m, exp10, [AC_DEFINE([HAVE_EXP10],[1],[Define to 1 if your system has 'exp10'.])])
#AC_FUNC_MKTIME

AH_BOTTOM([
#ifndef HAVE_TRUNC
#include <math.h>
inline static double trunc(double x)
{
  return x >= 0 ? floor(x) : ceil(x);
}
#endif

#ifndef HAVE_EXP10
#include <math.h>
inline static double exp10(double x)
{
  return pow(10.0, x);
}
#endif
])

GR_CHECK_SHM_OPEN
GR_LIBGNURADIO_CORE_EXTRA_LDFLAGS
GR_CHECK_CREATEFILEMAPPING

dnl Check for Mingw support
GR_PWIN32

dnl Do we have "dot", part of the graphviz package from AT&T?
dnl Doxgen will use it to draw pretty diagrams ;-)
AC_CHECK_PROG(HAVE_DOT, [dot],[YES],[NO])

PKG_CHECK_MODULES(FFTW3F, fftw3f >= 3.0)
AC_SUBST(FFTW3F_LIBS)

dnl conditional build stuff
GR_CHECK_DOXYGEN
GR_SET_MD_CPU

dnl Define where to look for cppunit includes and libs
dnl sets CPPUNIT_CFLAGS and CPPUNIT_LIBS
AM_PATH_CPPUNIT([1.9.14],[],
		[AC_MSG_ERROR([GNU Radio requires cppunit.  Stop])])

CPPUNIT_INCLUDES=$CPPUNIT_CFLAGS
AC_SUBST(CPPUNIT_INCLUDES)

dnl Define where to find boost includes
dnl defines BOOST_CFLAGS
GR_REQUIRE_BOOST_INCLUDES

DEFINES=""
AC_SUBST(DEFINES)

dnl Component specific configuration
dnl The order of the GR_ macros determines the order of compilation
AC_ARG_ENABLE(
    [all-components],
    [  --enable-all-components Build all components or stop on failed dependencies]
)

build_dirs="config"
GRC_GNURADIO_CORE
GRC_GNURADIO_EXAMPLES
GRC_USRP
GRC_GR_USRP			dnl this must come after GRC_USRP
GRC_GR_AUDIO_ALSA
GRC_GR_AUDIO_JACK
GRC_GR_AUDIO_OSS
GRC_GR_AUDIO_OSX
GRC_GR_AUDIO_PORTAUDIO
GRC_GR_AUDIO_WINDOWS
GRC_GR_ATSC
GRC_GR_COMEDI
GRC_GR_ERROR_CORRECTING_CODES
GRC_GR_GSM_FR_VOCODER
GRC_GR_RADAR
GRC_GR_RADIO_ASTRONOMY
GRC_GR_TRELLIS
GRC_GR_VIDEO_SDL
GRC_GR_WXGUI
GRC_PMT
GRC_MBLOCK			dnl this must come after GRC_PMT
GRC_EZDOP		
GRC_GR_EZDOP			dnl this must come after GRC_EZDOP
GRC_GR_RDF
GRC_GR_PAGER

# Each component is now either to be built, was skipped, or failed dependencies
AC_SUBST([build_dirs], [$build_dirs])
AC_SUBST([skipped_dirs], [$skipped_dirs])
 
AC_CONFIG_FILES([\
    Makefile \
    run_tests.sh
    config/Makefile
])

dnl run_tests.sh is created from run_tests.in.sh.  Make it executable.
AC_CONFIG_COMMANDS([run_tests_build], [chmod +x run_tests.sh])

AC_OUTPUT

echo
echo "*********************************************************************"
echo The following GNU Radio components have been successfully configured:
echo 
for dir in $build_dirs
do
    echo $dir
done
echo
echo You my now run the 'make' command to build these components.
echo
if test "$skipped_dirs" != ""; then
    echo "*********************************************************************"
    echo The following components were skipped either because you asked not
    echo to build them or they didn\'t pass configuration checks:
    echo
    for dir in $skipped_dirs
    do
        echo $dir
    done
    echo
    echo These components will not be built.
    echo
fi
