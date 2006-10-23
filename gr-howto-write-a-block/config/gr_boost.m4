dnl
dnl Copyright 2004,2005 Free Software Foundation, Inc.
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
dnl the Free Software Foundation, Inc., 51 Franklin Street,
dnl Boston, MA 02110-1301, USA.
dnl

dnl This tries to do the "right thing" to locate the boost include files.
dnl If the user explicitly specified --with-boost-include-dir=<foo>
dnl we believe them and use it.  Otherwise,
dnl
dnl We look for boost/shared_ptr.hpp in the "normal places".  That is,
dnl wherever AC_CHECK_HEADER looks.  If the boost includes are in /usr/local/include
dnl this step will find them.
dnl
dnl Otherwise, we check to see if the boost stuff was installed in a version-specific
dnl directory under /usr/local/include.  These look like: /usr/local/include/boost-1_33_1
dnl If there's more than one version installed, we select the
dnl lexicographically greatest one.
dnl
dnl If none of these work, we bail.

AC_DEFUN([GR_REQUIRE_BOOST_INCLUDES],
[
  AC_LANG_PUSH(C++)
  gr_boost_include_dir=
  AC_ARG_WITH([boost-include-dir],
    AC_HELP_STRING([--with-boost-include-dir=<path>],
	           [path to boost c++ include files]),
    [
      # "yes" and "no" are bogus answers
      if test x"$with_boost_include_dir" = xyes ||
         test x"$with_boost_include_dir" = xno; then
	gr_boost_include_dir=
      else
        gr_boost_include_dir=$with_boost_include_dir
      fi
    ])
  echo "gr_boost_include_dir = $gr_boost_include_dir"
  if test x$gr_boost_include_dir != x; then
    #
    # If the user specified a directory, then we use it
    #
    OLD_CPPFLAGS=$CPPFLAGS
    CPPFLAGS="$CPPFLAGS -I$gr_boost_include_dir"
    AC_CHECK_HEADER([boost/shared_ptr.hpp],
      [BOOST_CFLAGS="-I$gr_boost_include_dir"],
      [AC_MSG_ERROR(
        [Failed to locate boost/shared_ptr.hpp.
Try using --with-boost-include-dir=<path>, 
E.g., --with-boost-include-dir=/usr/local/include/boost-1_33_1])])
    CPPFLAGS=$OLD_CPPFLAGS
  else
    #
    # Otherwise we check in the default places
    # 
    AC_CHECK_HEADER([boost/shared_ptr.hpp],
      [BOOST_CFLAGS=""],
      [ # Nope, look for latest version if any in $prefix/include/boost-*

        # Wipe out cached value.  KLUDGE: AC should have API for this
	unset AS_TR_SH([ac_cv_header_boost/shared_ptr.hpp])

	boost_last_match(){
	  #echo "boost_last_match: [$]*"
	  pattern="[$]1"
	  shift
	  if test "[$]pattern" = "[$]1"
	  then
	    LM=''
	  else
	    shift `expr [$]# - 1`
	    LM=[$]1
	  fi
	  #echo "LM(1)='[$]LM'"
	}

	pattern="/usr/local/include/boost-*"
	boost_last_match "$pattern" $pattern
	#echo "LM(2)='$LM'"

        OLD_CPPFLAGS=$CPP_FLAGS
        CPPFLAGS="$CPPFLAGS -I$LM"
        AC_CHECK_HEADER([boost/shared_ptr.hpp],
          [BOOST_CFLAGS="-I$LM"],
          [AC_MSG_ERROR(
            [Failed to locate boost/shared_ptr.hpp.  
Try using --with-boost-include-dir=<path>, 
E.g., --with-boost-include-dir=/usr/local/include/boost-1_33_1])])
        CPPFLAGS=$OLD_CPPFLAGS
      ])

   fi
   unset boost_last_match LM
   AC_LANG_POP
   AC_SUBST(BOOST_CFLAGS)
])
