dnl Copyright 2009,2010 Free Software Foundation, Inc.
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


AC_DEFUN([GR_GIT],[
  dnl Identify git binary
  AC_PATH_PROG([GIT],[git])
  
  dnl If it exists, get either 'git describe' or fallback to current commit
  if test x$GIT != x ; then
    AC_MSG_CHECKING([existence of git version control directory])
    if test -d $srcdir/.git ; then
      AC_MSG_RESULT([ok])
      AC_MSG_CHECKING([git description of current commit])
      if (cd $srcdir && $GIT describe >/dev/null 2>&1); then
        GIT_DESCRIBE=`cd $srcdir && $GIT describe --abbrev=8 --long`
        # Release candidate tags create an extra -rcX field
	case $GIT_DESCRIBE in
	  *-*-*-*)
	    GIT_TAG=`echo $GIT_DESCRIBE | cut -f -2 -d '-'`
	    GIT_SEQNO=`echo $GIT_DESCRIBE | cut -f 3 -d '-'`
	    GIT_COMMIT=`echo $GIT_DESCRIBE | cut -f 4 -d '-' | cut -f 2- -d 'g'`
	    ;;
	  *-*-*)
	    GIT_TAG=`echo $GIT_DESCRIBE | cut -f 1 -d '-'`
 	    GIT_SEQNO=`echo $GIT_DESCRIBE | cut -f 2 -d '-'`
	    GIT_COMMIT=`echo $GIT_DESCRIBE | cut -f 3 -d '-' | cut -f 2- -d 'g'`
	    ;;
	esac

	AC_MSG_RESULT([$GIT_DESCRIBE])
      else
        AC_MSG_RESULT([no tag in history, using current commit])
	GIT_TAG=''
	GIT_SEQNO=''
	GIT_COMMIT=`cd $srcdir && $GIT describe --always --abbrev=8`
      fi
    else
      AC_MSG_RESULT([not found])
    fi

    AC_SUBST([GIT_DESCRIBE])
    AC_SUBST([GIT_TAG])
    AC_SUBST([GIT_SEQNO])
    AC_SUBST([GIT_COMMIT])
  fi
])
