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

AC_DEFUN([GR_VERSION],[
  dnl Computed version based on version.sh
  dnl Does not force recompile on rev change
  dnl
  dnl Source the variables describing the release version
  dnl 
  dnl MAJOR_VERSION          Major release generation (2.x, 3.x, etc.)
  dnl API_COMPAT             API compatibility version (3.2.x, 3.3.x, etc.)
  dnl MINOR_VERSION          Minor release version (3.3.0, 3.3.1, etc.)
  dnl MAINT_VERSION          Pure bugfix additions to make maintenance release
  dnl
  dnl The last two fields can have 'git' instead of a number to indicate
  dnl that this branch is between versions.
  . $srcdir/version.sh
  
  dnl Get git version if available
  GR_GIT

  dnl Test if we should use git version
  if test "$MINOR_VERSION" == "git"; then
    dnl RELEASE: 3.3git-xxx-gxxxxxxxx
    dnl DOCVER:  3.3git
    dnl LIBVER:  3.3git
    RELEASE=$GIT_DESCRIBE
    DOCVER=$MAJOR_VERSION.$API_COMPAT$MINOR_VERSION
    LIBVER=$MAJOR_VERSION.$API_COMPAT$MINOR_VERSION
  else
    if test "$MAINT_VERSION" == "git" ; then
      dnl RELEASE: 3.3.1git-xxx-gxxxxxxxx
      dnl DOCVER:  3.3.1git
      dnl LIBVER:  3.3.1git
      RELEASE=$GIT_DESCRIBE
      DOCVER=$MAJOR_VERSION.$API_COMPAT.$MINOR_VERSION$MAINT_VERSION
      LIBVER=$MAJOR_VERSION.$API_COMPAT.$MINOR_VERSION$MAINT_VERSION
    else
      dnl This is a numbered release.
      dnl RELEASE: 3.3.1{.x}
      dnl DOCVER:  3.3.1{.x}
      dnl LIBVER:  3.3.1{.x}
      RELEASE=$MAJOR_VERSION.$API_COMPAT.$MINOR_VERSION
      if test "$MAINT_VERSION" != "0"; then
        RELEASE=$RELEASE.$MAINT_VERSION
      fi

      DOCVER=$RELEASE
      LIBVER=$RELEASE
    fi
  fi

  AC_MSG_NOTICE([GNU Radio Release $RELEASE])
  AC_SUBST(RELEASE)
  AC_SUBST(DOCVER)
  AC_SUBST(LIBVER)
])
