dnl
dnl Copyright 2007 Free Software Foundation, Inc.
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

# GR_SUBVERSION()
#
# Test for presence of subversion, and create variables for 
# current repository version and last changed date.

AC_DEFUN([GR_SUBVERSION],[
    	AC_PATH_PROG([SVN],[svn])
	if test "$SVN" != "" -a -d .svn ; then
	    SVNVERSION=`$SVN info . | grep '^Revision' | cut -f 2- -d ' '`
	    SVNDATE=`$SVN info . | grep 'Last Changed Date' | cut -f 4-6 -d ' '`
	fi

	AC_SUBST(SVNVERSION)
	AC_SUBST(SVNDATE)
])
