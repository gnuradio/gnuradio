dnl
dnl Copyright 2003 Free Software Foundation, Inc.
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

# SWIG_PROG([required-version])
#
# Checks for the SWIG program.  If found you can (and should) call SWIG via $(SWIG).
# You can use the optional first argument to check if the version of the available SWIG
# is greater or equal to the value of the argument.  It should have the format:
# N[.N[.N]] (N is a number between 0 and 999.  Only the first N is mandatory.)
AC_DEFUN([SWIG_PROG],[
	AC_REQUIRE([AC_PROG_MAKE_SET])
	AC_CHECK_PROG(SWIG,swig,[`which swig`])
	if test -z "$SWIG" ; then
		AC_MSG_ERROR([Cannot find 'swig' program.  SWIG version >= $1 required])
		SWIG=false
	elif test -n "$1" ; then
		AC_MSG_CHECKING([for SWIG version])
		swig_version=`$SWIG -version 2>&1 | \
			awk '/^SWIG Version [[0-9]+\.[0-9]+\.[0-9]]+.*$/ { split($[3],a,"[[^.0-9]]"); print a[[1]] }'`
		AC_MSG_RESULT([$swig_version])
		if test -n "$swig_version" ; then
			swig_version=`echo $swig_version | \
				awk '{ split($[1],a,"\."); print [a[1]*1000000+a[2]*1000+a[3]] }' 2>/dev/null`
			swig_required_version=`echo $1 | \
				awk '{ split($[1],a,"\."); print [a[1]*1000000+a[2]*1000+a[3]] }' 2>/dev/null`
			if test $swig_required_version -gt $swig_version ; then
				AC_MSG_ERROR([SWIG version >= $1 required])
			fi
		else
			AC_MSG_ERROR([cannot determine SWIG version])
		fi
	fi
])

# SWIG_ENABLE_CXX()
#
# Enable swig C++ support.  This effects all invocations of $(SWIG).
AC_DEFUN([SWIG_ENABLE_CXX],[
	AC_REQUIRE([SWIG_PROG])
	AC_REQUIRE([AC_PROG_CXX])
	if test "$SWIG" != "false" ; then
		SWIG="$SWIG -c++"
	fi
])

# SWIG_PYTHON([use-shadow-classes])
#
# Checks for Python and provides the $(SWIG_PYTHON_CPPFLAGS),
# $(SWIG_PYTHON_LIB) and $(SWIG_PYTHON_OPT) output variables.
# $(SWIG_PYTHON_OPT) contains all necessary swig options to generate
# code for Python.  If you need multi module support use
# $(SWIG_PYTHON_LIB) (provided by the SWIG_MULTI_MODULE_SUPPORT()
# macro) to link against the appropriate library.  It contains the
# SWIG Python runtime library that is needed by the type check system
# for example.

AC_DEFUN([SWIG_PYTHON],[
	AC_REQUIRE([SWIG_PROG])
	AC_REQUIRE([PYTHON_DEVEL])
	if test "$SWIG" != "false" ; then
		AC_SUBST(SWIG_PYTHON_LIB,[-lswigpy])
dnl		test ! "x$1" = "xno" && swig_shadow=" -shadow" || swig_shadow=""
dnl		AC_SUBST(SWIG_PYTHON_OPT,[-python$swig_shadow])
		AC_SUBST(SWIG_PYTHON_OPT,[-python])
	fi
	AC_SUBST(SWIG_PYTHON_CPPFLAGS,[$PYTHON_CPPFLAGS])
])
