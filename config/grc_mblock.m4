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
dnl the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
dnl Boston, MA 02111-1307, USA.

AC_DEFUN([GRC_MBLOCK],[
    GRC_ENABLE([mblock])

    AC_CONFIG_FILES([\
	mblock/Makefile \
	mblock/doc/Makefile \
	mblock/src/Makefile \
	mblock/src/lib/Makefile
    ])

    passed=yes
    # Don't do mblock if pmt skipped
    # There *has* to be a better way to check if a value is in a string
    for dir in $skipped_dirs
    do
	if test x$dir = xpmt; then
	    AC_MSG_RESULT([Component mblock requires pmt, which is not being built.])
	    passed=no
	fi
    done

    GRC_BUILD_CONDITIONAL([mblock],[
        dnl run_tests is created from run_tests.in.  Make it executable.
	dnl AC_CONFIG_COMMANDS([run_tests_mblock], [chmod +x mblock/src/python/run_tests])
    ])
])
