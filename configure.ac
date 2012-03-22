dnl Copyright 2001-2011 Free Software Foundation, Inc.
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

AC_INIT
AC_PREREQ(2.57)

dnl Set the prefix to the default when --prefix is not specified.
dnl This is critical for variable substitutions in the configure.
if test "${prefix}" = "NONE"; then
    prefix=${ac_default_prefix}
fi

AM_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR([gnuradio-core/src/lib/runtime/gr_vmcircbuf.cc])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

GR_VERSION
dnl ustar required to have pathnames > 99 chars
_AM_SET_OPTION([tar-ustar])
AM_INIT_AUTOMAKE(gnuradio,$RELEASE)

DEFINES=""
AC_SUBST(DEFINES)

dnl Remember if the user explicity set CFLAGS
if test -n "${CFLAGS}"; then
  user_set_cflags=yes
fi
dnl Remember if the user explicity set CXXFLAGS
if test -n "${CXXFLAGS}"; then
  user_set_cxxflags=yes
fi


LF_CONFIGURE_CC
LF_CONFIGURE_CXX
GR_LIB64		dnl check for lib64 suffix after choosing compilers

dnl The three macros above are known to override CFLAGS if the user
dnl didn't specify them.  Though I'm sure somebody thought this was
dnl a good idea, it makes it hard to use other than -g -O2 when compiling
dnl selected files.  Thus we "undo" the damage here...
dnl 
dnl If the user specified CFLAGS, we use them.
dnl See Makefile.common for the rest of the magic.
if test "$user_set_cflags" != yes; then
  autoconf_default_CFLAGS="$CFLAGS"
  CFLAGS=""
fi
AC_SUBST([autoconf_default_CFLAGS])


dnl The three macros above are known to override CXXFLAGS if the user
dnl didn't specify them.  Though I'm sure somebody thought this was
dnl a good idea, it makes it hard to use other than -g -O2 when compiling
dnl selected files.  Thus we "undo" the damage here...
dnl 
dnl If the user specified CXXFLAGS, we use them.  Otherwise when compiling
dnl the output of swig use use -O1 if we're using g++.
dnl See Makefile.common for the rest of the magic.
if test "$user_set_cxxflags" != yes; then
  autoconf_default_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS=""
  if test "$GXX" = yes; then
    case "$host_cpu" in
    	powerpc*)
	   dnl "-O1" is broken on the PPC for some reason
	   dnl (at least as of g++ 4.1.1)
	   swig_CXXFLAGS="-g1 -O2 -Wno-strict-aliasing -Wno-parentheses"
	;;
    	*) 
	swig_CXXFLAGS="-g -O1 -Wno-strict-aliasing -Wno-parentheses"
	;;
    esac
  fi
fi
AC_SUBST(autoconf_default_CXXFLAGS)
AC_SUBST(swig_CXXFLAGS)

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

# AC_PROG_MKDIR_P
# is a backport of autoconf-2.60's AC_PROG_MKDIR_P.
# Remove this macro when we can assume autoconf >= 2.60.
m4_ifdef([AC_PROG_MKDIR_P], [], [
  AC_DEFUN([AC_PROG_MKDIR_P],
    [AC_REQUIRE([AM_PROG_MKDIR_P])dnl defined by automake
     MKDIR_P='$(mkdir_p)'
     AC_SUBST([MKDIR_P])])
])
AC_PROG_MKDIR_P

AC_PATH_PROG([RM_PROG], [rm])

AC_LIBTOOL_WIN32_DLL
dnl AC_DISABLE_SHARED	dnl don't build shared libraries
AC_ENABLE_SHARED	dnl do build shared libraries
AC_DISABLE_STATIC	dnl don't build static libraries
m4_ifdef([LT_INIT],[LT_INIT],[AC_PROG_LIBTOOL])
dnl GR_FORTRAN

GR_NO_UNDEFINED		dnl do we need the -no-undefined linker flag
GR_SCRIPTING

# Allow user to choose whether to generate SWIG/Python 
# Default is enabled
AC_ARG_ENABLE([python],
  [AS_HELP_STRING([--enable-python],
    [generate SWIG/Python components (default is yes)])],
  [case "${enableval}" in
     yes) enable_python=yes ;;
     no) enable_python=no ;;
     *) AC_MSG_ERROR([bad value ${enableval} for --enable-python]) ;;
   esac],
  [enable_python=yes]
)

# Allow user to choose whether to generate SWIG/Guile
# Default is disabled
AC_ARG_ENABLE([guile],
  [AS_HELP_STRING([--enable-guile],
    [generate SWIG/Guile components (default is no)])],
  [case "${enableval}" in
     yes) enable_guile=yes ;;
     no) enable_guile=no ;;
     *) AC_MSG_ERROR([bad value ${enableval} for --enable-guile]) ;;
   esac],
  [enable_guile=no]
)

AM_CONDITIONAL([PYTHON], [test x$enable_python = xyes])
AM_CONDITIONAL([GUILE], [test x$enable_guile = xyes])

AC_CHECK_PROG([XMLTO],[xmlto],[yes],[])
AM_CONDITIONAL([HAS_XMLTO], [test x$XMLTO = xyes])

dnl Checks for libraries.
AC_CHECK_LIB(socket,socket)

dnl Set the c++ compiler that we use for the build system when cross compiling
if test x$CXX_FOR_BUILD = x
then
  if test x$cross_compiling = xno; then
    CXX_FOR_BUILD=${CXX}
  else
    CXX_FOR_BUILD=g++
  fi
fi
AC_SUBST(CXX_FOR_BUILD)

dnl Check for SysV shm (mandatory)
dnl FIXME this is no longer mandatory.  Check the macro.
GR_SYSV_SHM

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(fcntl.h limits.h strings.h time.h sys/ioctl.h sys/time.h unistd.h)
AC_CHECK_HEADERS(linux/ppdev.h dev/ppbus/ppi.h sys/mman.h sys/select.h sys/types.h)
AC_CHECK_HEADERS(sys/resource.h stdint.h sched.h signal.h sys/syscall.h malloc.h)
AC_CHECK_HEADERS(windows.h)
AC_CHECK_HEADERS(vec_types.h)
AC_CHECK_HEADERS(netdb.h netinet/in.h arpa/inet.h sys/types.h sys/socket.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_BIGENDIAN([GR_ARCH_BIGENDIAN=1],[GR_ARCH_BIGENDIAN=0])
AC_SUBST(GR_ARCH_BIGENDIAN)
AC_STRUCT_TM

dnl Checks for library functions.
AC_FUNC_ALLOCA
GR_CHECK_MEMALIGN
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([mmap select socket strcspn strerror strspn getpagesize sysconf])
AC_CHECK_FUNCS([snprintf gettimeofday nanosleep sched_setscheduler])
AC_CHECK_FUNCS([modf sqrt sigaction sigprocmask pthread_sigmask])
AC_CHECK_FUNCS([sched_setaffinity])

AC_CHECK_LIB(m, sincos, [AC_DEFINE([HAVE_SINCOS],[1],[Define to 1 if your system has `sincos'.])])
AC_CHECK_LIB(m, sincosf,[AC_DEFINE([HAVE_SINCOSF],[1],[Define to 1 if your system has `sincosf'.])])
AC_CHECK_LIB(m, sinf, [AC_DEFINE([HAVE_SINF],[1],[Define to 1 if your system has `sinf'.])])
AC_CHECK_LIB(m, cosf, [AC_DEFINE([HAVE_COSF],[1],[Define to 1 if your system has `cosf'.])])
AC_CHECK_LIB(m, exp10, [AC_DEFINE([HAVE_EXP10],[1],[Define to 1 if your system has 'exp10'.])])
AC_CHECK_LIB(m, log2, [AC_DEFINE([HAVE_LOG2],[1],[Define to 1 if your system has 'log2'.])])
#AC_FUNC_MKTIME

AH_BOTTOM([
#ifndef HAVE_EXP10
#include <math.h>
inline static double exp10(double x)
{
  return pow(10.0, x);
}
#endif

#ifdef HAVE_WINDOWS_H
#define NOMINMAX
#endif
])

GR_VMCIRCBUF
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
dnl Try using pkg-config first, then fall back to cppunit-config.
PKG_CHECK_EXISTS(cppunit,
  [PKG_CHECK_MODULES(CPPUNIT, cppunit >= 1.9.14)],
  [AM_PATH_CPPUNIT([1.9.14],[],
		[AC_MSG_ERROR([GNU Radio requires cppunit.  Stop])])])

CPPUNIT_INCLUDES=$CPPUNIT_CFLAGS
AC_SUBST(CPPUNIT_INCLUDES)

dnl see if GUILE is installed
dnl AC_PATH_PROG(GUILE,guile)

dnl
dnl We require the boost headers, thread lib and date_time lib.
dnl AX_BOOST_BASE finds the headers and the lib dir (-L<foo>)
dnl
dnl calls AC_SUBST(BOOST_CPPFLAGS), AC_SUBST(BOOST_LDFLAGS) and defines HAVE_BOOST
dnl
dnl Current Boost version requirement is >=1.35 for all platforms except Darwin,
dnl which requires >=1.37 for code in usrp host library.
case "$host_os" in
     darwin*)
        AX_BOOST_BASE([1.37])
        ;;
      *)
        AX_BOOST_BASE([1.35])
        ;;
esac

dnl calls AC_SUBST(BOOST_THREAD_LIB), AC_SUBST(BOOST_CXXFLAGS) and defines HAVE_BOOST_THREAD
AX_BOOST_THREAD
CXXFLAGS="$CXXFLAGS $BOOST_CXXFLAGS"       dnl often picks up a -pthread or something similar
CFLAGS="$CFLAGS $BOOST_CXXFLAGS"           dnl often picks up a -pthread or something similar

dnl
dnl all the rest of these call AC_SUBST(BOOST_<foo>_LIB) and define HAVE_BOOST_<foo>
dnl
AX_BOOST_DATE_TIME
AX_BOOST_FILESYSTEM
dnl AX_BOOST_IOSTREAMS
AX_BOOST_PROGRAM_OPTIONS
dnl AX_BOOST_REGEX
dnl AX_BOOST_SERIALIZATION
dnl AX_BOOST_SIGNALS
AX_BOOST_SYSTEM
dnl AX_BOOST_TEST_EXEC_MONITOR
dnl AX_BOOST_UNIT_TEST_FRAMEWORK
dnl AX_BOOST_WSERIALIZATION

BUILD_DATE=`date -u +"%a, %d %b %Y %R:%S"`
AC_SUBST(BUILD_DATE)

dnl SYSCONFDIR substitution
if test "${sysconfdir}" == "\${prefix}/etc" ; then
   if test "${prefix}" == "NONE" ; then
      SYSCONFDIR=["$ac_default_prefix/etc"]
   else
      SYSCONFDIR=["${prefix}/etc"]
   fi
else
   SYSCONFDIR=[${sysconfdir}]
fi
AC_SUBST(SYSCONFDIR)

dnl System configuration files
GR_PREFSDIR=[$SYSCONFDIR/${PACKAGE}/conf.d]
AC_SUBST(GR_PREFSDIR)

dnl Component specific configuration
dnl The order of the GR_ macros determines the order of compilation
dnl For -any- checks on $enable_all_components
dnl use the following guidelines:
dnl   yes : --enable-all-components was specified, so error out if any
dnl         components do not pass configuration checks.
dnl   no  : --disable-all-components was specified, so try to build the
dnl         --enable'd components, and error out if any do not pass
dnl         configuration checks.
dnl   ""  : this option was not specified on the command line; try to
dnl         build all components that are not --with'd, but don't
dnl         error out if any component does not pass configuration checks.
dnl
dnl For each --enable-foo component, if that flag is not specified on
dnl the command line, the related variable $enable_foo will be set to
dnl $enable_all_components .

AC_ARG_ENABLE(
    [all-components],
    [  --enable-all-components Build all configurable components (default), or stop on failed dependencies]
)

build_dirs="config"
GRC_GRUEL                       dnl must come first
GRC_VOLK
GRC_GNURADIO_CORE
GRC_GR_DIGITAL
GRC_GR_AUDIO
GRC_GR_VOCODER
GRC_GR_ATSC
GRC_GR_COMEDI
GRC_GR_NOAA
GRC_GR_PAGER
GRC_GR_TRELLIS
GRC_GR_VIDEO_SDL
GRC_GR_WXGUI
GRC_GR_QTGUI
GRC_GR_UTILS                    dnl this must come after GRC_GR_WXGUI
GRC_GNURADIO_EXAMPLES		dnl must come after all GRC_GR_*
GRC_GRC
GRC_GR_UHD
GRC_GR_SHD
GRC_GR_FCD
GRC_DOCS			dnl must be last

# Each component is now either to be built, was skipped, will be
# included from pre-installed libraries and includes, or failed
# dependencies.
AC_SUBST([build_dirs], [$build_dirs])
AC_SUBST([skipped_dirs], [$skipped_dirs])
AC_SUBST([with_dirs], [$with_dirs])

# fix for older autotools that don't define these by default
AC_SUBST(abs_top_srcdir)
AC_SUBST(abs_top_builddir)
AC_SUBST(MKDIR_P)

# 'with' variables - the pre-installed libraries, includes, and paths
# - must always come last in the lists, so they require special
# treatment.
AC_SUBST(with_INCLUDES)
AC_SUBST(with_SWIG_INCLUDES)
AC_SUBST(with_PYDIRPATH)
AC_SUBST(with_SWIGDIRPATH)
AC_SUBST(with_LIBDIRPATH)
AC_SUBST(with_GUILE_LOAD_PATH)

# Local files tweaked by AC
AC_CONFIG_FILES([\
    Makefile \
    run_tests.sh \
    setup_guile_test_env \
    config/Makefile
])

dnl run_tests.sh is created from run_tests.sh.in .  Make it executable.
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
if test "$with_dirs" != ""; then
    echo "*********************************************************************"
    echo The following components will be included from pre-installed
    echo libraries and includes:
    echo
    for dir in $with_dirs
    do
        echo $dir
    done
    echo
    echo These components will not be built.
    echo
fi

echo Configured GNU Radio release $RELEASE for build.
