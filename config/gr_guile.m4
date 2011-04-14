dnl
dnl Copyright 2003,2004,2005 Free Software Foundation, Inc.
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
dnl 

# GUILE_DEVEL()
#
# Checks for Guile and tries to get the include path to 'Guile.h'.
# It sets the $(GUILE_CPPFLAGS), $(GUILE_LDFLAGS) and $(guiledir) output variables,
#
AC_DEFUN([GUILE_DEVEL],[
    dnl see if GUILE is installed
    AC_PATH_PROG(GUILE, guile)
    dnl get the config program
    AC_PATH_PROG(GUILE_CONFIG, guile-config)
    if test x${GUILE_CONFIG} != x; then
      GUILE_CFLAGS="`${GUILE_CONFIG} compile`"
      GUILE_LIBS="`${GUILE_CONFIG} link`"
      GUILE_PKLIBDIR="`${GUILE_CONFIG} info pkglibdir`"
      GUILE_PKDATADIR="`${GUILE_CONFIG} info pkgdatadir`/site"
    else
      dnl Only break on an error if we are using guile.
      dnl This should be taken care of in gr_scripting.m4 and I don't know why it's not.
      if test x${enable_guile} = xyes; then
         AC_MSG_ERROR([You need to install the guile development files (e.g., libguile-dev, guile-devel, etc.)!])
      fi
      GUILE_CFLAGS=""
      GUILE_PKLIBDIR=""
      GUILE_LIBS="Wl,-Bsymbolic-functions -lguile -lgmp -lcrypt"
    fi
    AC_SUBST(GUILE_CFLAGS)
    AC_SUBST(GUILE_PKLIBDIR)
    AC_SUBST(GUILE_PKDATADIR)
    AC_SUBST(GUILE_LIBS)

    dnl see if we've got lt_dladvise_global (libtool-2.*)
    AC_SEARCH_LIBS([lt_dladvise_global],[ltdl],
		   [AC_DEFINE([HAVE_LT_DLADVISE_GLOBAL], [1],
		              [Define if lt_dladvise_global exists.])],
		   [],[])
])

# GUILE_CHECK_MODULE
#
# Determines if a particular Guile module can be imported
#
# $1 - module name
# $2 - module description
# $3 - action if found
# $4 - action if not found
# $5 - test command

AC_DEFUN([GUILE_CHECK_MODULE],[
    AC_MSG_CHECKING([for $2])
    dnl ########################################
    dnl # import and test checking
    dnl ########################################
  dnl if test x${enable_guile} = xyes; then
  dnl fi
])
