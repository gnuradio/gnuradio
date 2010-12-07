dnl all this stuff taken and modified from GNURADIO!
dnl
dnl LV_CONFIGURE
dnl
dnl Handles the bulk of the configure.ac work for an out-of-tree build
dnl
dnl You must invoke:
dnl
dnl   AC_INIT(package_name,version)
dnl   AC_PREREQ(2.57)
dnl   AC_CONFIG_AUX_DIR([.])	
dnl
dnl in configure.ac before LV_CONFIGURE
dnl
dnl
dnl N.B., this is an m4_define because if it were an AC_DEFUN it would
dnl get called too late to be useful.

m4_define([LV_CONFIGURE],
[

  AC_CONFIG_SRCDIR([config/lv_configure.m4])
  AC_CONFIG_AUX_DIR([.])	
  AM_CONFIG_HEADER(config.h)

  AC_CANONICAL_BUILD
  AC_CANONICAL_HOST
  AC_CANONICAL_TARGET

  AM_INIT_AUTOMAKE(libvector,0.0svn)

  LF_CONFIGURE_CC
  LF_CONFIGURE_CXX
  GR_LIB64		dnl check for lib64 suffix after choosing compilers

  LV_GCC_VERSION_WORKAROUND
  dnl add ${prefix}/lib${gr_libdir_suffix}/pkgconfig to the head of the PKG_CONFIG_PATH
  if test x${PKG_CONFIG_PATH} = x; then
      PKG_CONFIG_PATH=${prefix}/lib${gr_libdir_suffix}/pkgconfig
  else
      PKG_CONFIG_PATH=${prefix}/lib${gr_libdir_suffix}/pkgconfig:${PKG_CONFIG_PATH}
  fi
  export PKG_CONFIG_PATH

  LF_SET_WARNINGS
dnl  GR_SET_GPROF
dnl  GR_SET_PROF
  AM_PROG_AS
  AC_PROG_LN_S
  AC_PROG_MAKE_SET
  AC_PROG_INSTALL
  AC_PATH_PROG([RM_PROG], [rm])

  AC_LIBTOOL_WIN32_DLL
  AC_ENABLE_SHARED	dnl do build shared libraries... important for qa
  AC_DISABLE_STATIC	dnl don't build static libraries... important for qa
  m4_ifdef([LT_INIT],[LT_INIT],[AC_PROG_LIBTOOL])
  dnl GR_FORTRAN

  GR_NO_UNDEFINED	dnl do we need the -no-undefined linker flag
dnl  GR_SCRIPTING		dnl Locate python, SWIG, etc

dnl  AC_ARG_WITH([python],
dnl    AC_HELP_STRING([--with-python], [Should we use python? [[default=yes]]]),
dnl    [case "$with_python" in
dnl      (no | yes) ;;
dnl      (*) AC_MSG_ERROR([Invalid argument ($with_python) to --with-python]) ;;
dnl     esac],
dnl    [with_python=yes])

dnl  AM_CONDITIONAL([USE_PYTHON], [test "$with_python" = yes])


  dnl Set the c++ compiler that we use for the build system when cross compiling
  if test "x$CXX_FOR_BUILD" = x
  then
    CXX_FOR_BUILD=${CXX}
  fi
  AC_SUBST(CXX_FOR_BUILD)

  dnl Checks for header files.
  AC_HEADER_STDC

  dnl Checks for typedefs, structures, and compiler characteristics.
  AC_C_CONST
  AC_C_INLINE
  AC_TYPE_SIZE_T
  AC_HEADER_TIME
  AC_C_BIGENDIAN

  dnl Check for Mingw support
  GR_PWIN32
  GR_LIBGNURADIO_CORE_EXTRA_LDFLAGS
  
  LDFLAGS="$LDFLAGS $LIBGNURADIO_CORE_EXTRA_LDFLAGS"

  AC_CHECK_PROG([XMLTO],[xmlto],[yes],[])
  AM_CONDITIONAL([HAS_XMLTO], [test x$XMLTO = xyes])

  dnl Define where to look for cppunit includes and libs
  dnl sets CPPUNIT_CFLAGS and CPPUNIT_LIBS
  dnl Try using pkg-config first, then fall back to cppunit-config.
  PKG_CHECK_EXISTS(cppunit,
    [PKG_CHECK_MODULES(CPPUNIT, cppunit >= 1.9.14)],
    [AM_PATH_CPPUNIT([1.9.14],[],
		     [AC_MSG_ERROR([LIBVECTOR requires cppunit.  Stop])])])

dnl  PKG_CHECK_MODULES(GNURADIO_CORE, gnuradio-core >= 3)
dnl  LIBS="$LIBS $GNURADIO_CORE_LIBS"
])
