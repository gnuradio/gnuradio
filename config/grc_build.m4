dnl Copyright 2006,2008 Free Software Foundation, Inc.
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

# Create --enable-foo argument for named component, create variables as needed
# $1 is component name
AC_DEFUN([GRC_ENABLE],[
    AC_ARG_ENABLE([$1],
		  AC_HELP_STRING([--enable-$1],
				 [Fail build if $1 fails configuration]),
		  [],
                  [[enable_]m4_bpatsubst($1,-,_)=$enable_all_components])
])

# Create --with-foo argument for named compoment, create variables as needed
# $1 is component name
AC_DEFUN([GRC_WITH],[
    AC_ARG_WITH([$1],
	        AC_HELP_STRING([--with-$1],
	 		       [Use package $1 if installed in $prefix/lib/pkgconfig; otherwise revert back to --enable-$1]),
                [],
	        [])
])

# Add the specified "with" list; clear the provided variable
# $1 is the component name
# $2 is the path list name suffix
# $3 is the separator (for paths, ":"; for includes " ")
AC_DEFUN([GRC_ADD_TO_LIST],[
    if test "x${$1_$2}" != "x"; then
        if test "x$with_$2" = "x"; then
            with_$2="${$1_$2}"
	else
	    with_$2="${$1_$2}"$3"$with_$2"
	fi
	$1_$2=
    fi
])

# Conditionally build named component.
# $1 is component name
# $2 is executed if configuration passes and build is desired
AC_DEFUN([GRC_BUILD_CONDITIONAL],[
    _GRC_BUILD_CONDITIONAL($1, m4_bpatsubst($1,-,_), $2)
])

# $1 is the component name
# $2=m4_bpatsubst($1,-,_)
# $3 is executed if configuration passes and build is desired
# Use $passed=no to indicate configuration failure
# Use $passed=with to indicate the use of pre-installed libraries and headers
# Any other value of $passed, including blank, assumes success 
AC_DEFUN([_GRC_BUILD_CONDITIONAL],[
    if test x$passed = xno; then
	if test x$enable_$2 = xyes; then
	    AC_MSG_ERROR([Component $1 has errors, stopping.])
	else
	    AC_MSG_RESULT([Not building component $1.])
	fi
    else
        if test x$passed = xwith; then
	    with_dirs="$with_dirs $1"
	    GRC_ADD_TO_LIST($2, INCLUDES, " ")
	    GRC_ADD_TO_LIST($2, SWIG_INCLUDES, " ")
	    GRC_ADD_TO_LIST($2, PYDIRPATH, ":")
	    GRC_ADD_TO_LIST($2, SWIGDIRPATH, ":")
	    GRC_ADD_TO_LIST($2, LIBDIRPATH, ":")
	    AC_MSG_RESULT([Component $1 will be included from a pre-installed library and includes, and will not be built.])
	else
            if test x$enable_$2 != xno; then
	        $3
	        build_dirs="$build_dirs $1"
	        AC_MSG_RESULT([Component $1 passed configuration checks, building.])
            else
	        passed=no
	        AC_MSG_RESULT([Component $1 passed configuration checks, but not building.])
	    fi
	fi
    fi
    if test x$passed = xno; then
        skipped_dirs="$skipped_dirs $1"
        $2_skipped=yes
    else
        $2_skipped=no
    fi
])
