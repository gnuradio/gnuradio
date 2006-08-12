#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(hunter, 0.1svn, jcorgan@aeinet.com)
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src/hunter.cc])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h sys/ioctl.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_HEADER_STDC
AC_FUNC_MKTIME
AC_CHECK_FUNCS([modf sqrt])

# Application specific checks
HUNTER_WX
HUNTER_FTDI

AC_CONFIG_FILES([ \
    Makefile
    src/Makefile
])

AC_OUTPUT
