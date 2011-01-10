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

# TCL_DEVEL()
#
# Checks for Tcl and tries to get the include path to 'Tcl.h'.
# It sets the $(TCL_CPPFLAGS), $(TCL_LDFLAGS) and $(tcldir) output variables,
#
AC_DEFUN([TCL_DEVEL],[
    dnl see if tclsh is installed
    if test x${enable_tcl} = xyes; then
        AC_PATH_PROG(TCLSH,tclsh)

	AC_REQUIRE([AC_CANONICAL_HOST])

	AC_ARG_WITH(tcldir,
                    AC_HELP_STRING([--with-tcldir=DIR], 
                       [tcl installation directory (cross-compiling) [[default=$prefix/lib/tcl2.5/site-packages]]]),
		    [with_tcldir=${withval}],[with_tcldir=${prefix}/lib/tcl2.5/site-packages])

	# if we're cross-compiling, asking the host tcl about any of
	# this is completely useless...

	dnl if test x$cross_compiling != xno; then
	dnl 	tcldir=$with_tcldir
	dnl 	pyexecdir=$with_tcldir
	dnl 	AC_SUBST(TCL_CPPFLAGS)
	dnl 	AC_SUBST(TCL_LDFLAGS)
        dnl else
	dnl fi
    fi
])

# TCL_CHECK_MODULE
#
# Determines if a particular Tcl module can be imported
#
# $1 - module name
# $2 - module description
# $3 - action if found
# $4 - action if not found
# $5 - test command

AC_DEFUN([TCL_CHECK_MODULE],[
    AC_MSG_CHECKING([for $2])
    dnl ########################################
    dnl # import and test checking
    dnl ########################################
  dnl if test x${enable_tcl} = xyes; then
  dnl fi
])
