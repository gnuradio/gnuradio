# ===========================================================================
#             http://autoconf-archive.cryp.to/ax_boost_base.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_BASE([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the Boost C++ libraries of a particular version (or newer)
#
#   If no path to the installed boost library is given the macro searchs
#   under /usr, /usr/local, /opt and /opt/local and evaluates the
#   $BOOST_ROOT environment variable. Further documentation is available at
#   <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_CPPFLAGS) / AC_SUBST(BOOST_LDFLAGS)
#
#   And sets:
#
#     HAVE_BOOST
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Free Software Foundation, Inc.
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_BOOST_BASE],
[
AC_REQUIRE([GR_LIB64])
AC_ARG_WITH([boost],
    AS_HELP_STRING([--with-boost@<:@=DIR@:>@],
		   [use boost (default is yes) - it is possible to specify the root directory for boost (optional)]),
    [
    if test "$withval" = "no"; then
        want_boost="no"
    elif test "$withval" = "yes"; then
        want_boost="yes"
        ac_boost_path=""
    else
        want_boost="yes"
        ac_boost_path="$withval"
    fi
    ],
    [want_boost="yes"])


AC_ARG_WITH([boost-libdir],
        AS_HELP_STRING([--with-boost-libdir=LIB_DIR],
		       [Force given directory for boost libraries. Note that this
		        will overwrite library path detection, so use this parameter
		        only if default library detection fails and you know exactly
                        where your boost libraries are located.]),
        [
        if test -d $withval
        then
                ac_boost_lib_path="$withval"
        else
                AC_MSG_ERROR(--with-boost-libdir expected directory name)
        fi
        ],
        [ac_boost_lib_path=""]
)

if test "x$want_boost" = "xyes"; then
    boost_lib_version_req=ifelse([$1], ,1.20.0,$1)
    boost_lib_version_req_shorten=`expr $boost_lib_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
    boost_lib_version_req_major=`expr $boost_lib_version_req : '\([[0-9]]*\)'`
    boost_lib_version_req_minor=`expr $boost_lib_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
    boost_lib_version_req_sub_minor=`expr $boost_lib_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
    if test "x$boost_lib_version_req_sub_minor" = "x" ; then
        boost_lib_version_req_sub_minor="0"
        fi
    WANT_BOOST_VERSION=`expr $boost_lib_version_req_major \* 100000 \+  $boost_lib_version_req_minor \* 100 \+ $boost_lib_version_req_sub_minor`
    AC_MSG_CHECKING(for boost >= $boost_lib_version_req)
    succeeded=no

    dnl first we check the system location for boost libraries
    dnl this location ist chosen if boost libraries are installed with the --layout=system option
    dnl or if you install boost with RPM
    if test "$ac_boost_path" != ""; then
	dnl Look first where we think they ought to be, accounting for a possible "64" suffix on lib.
	dnl If that directory doesn't exist, fall back to the default behavior
	if test -d "$ac_boost_path/lib${gr_libdir_suffix}"; then
            BOOST_LDFLAGS="-L$ac_boost_path/lib${gr_libdir_suffix}"
        else
            BOOST_LDFLAGS="-L$ac_boost_path/lib"
        fi
        BOOST_CPPFLAGS="-I$ac_boost_path/include"
    else
        for ac_boost_path_tmp in /usr /usr/local /opt /opt/local ; do
            if test -d "$ac_boost_path_tmp/include/boost" && test -r "$ac_boost_path_tmp/include/boost"; then
		dnl Look first where we think they ought to be, accounting for a possible "64" suffix on lib.
		dnl If that directory doesn't exist, fall back to the default behavior
		if test -d "$ac_boost_path_tmp/lib${gr_libdir_suffix}"; then
                    BOOST_LDFLAGS="-L$ac_boost_path_tmp/lib${gr_libdir_suffix}"
		else
	            BOOST_LDFLAGS="-L$ac_boost_path_tmp/lib"
		fi
                BOOST_CPPFLAGS="-I$ac_boost_path_tmp/include"
                break;
            fi
        done
    fi

    dnl overwrite ld flags if we have required special directory with
    dnl --with-boost-libdir parameter
    if test "$ac_boost_lib_path" != ""; then
       BOOST_LDFLAGS="-L$ac_boost_lib_path"
    fi

    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
    export CPPFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
    export LDFLAGS

    AC_LANG_PUSH(C++)
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
    @%:@include <boost/version.hpp>
    ]], [[
    #if BOOST_VERSION >= $WANT_BOOST_VERSION
    // Everything is okay
    #else
    #  error Boost version is too old
    #endif
    ]])],[AC_MSG_RESULT(yes)
	  succeeded=yes
	  found_system=yes
          ],
         [])
    AC_LANG_POP([C++])
    CPPFLAGS="$CPPFLAGS_SAVED"
    LDFLAGS="$LDFLAGS_SAVED"


    dnl if we found no boost with system layout we search for boost libraries
    dnl built and installed without the --layout=system option
    if test "$succeeded" != "yes"; then
        _version=0

        if test "$ac_boost_path" != ""; then
	    path_list="$ac_boost_path"
	else
	    path_list="/usr /usr/local /opt /opt/local"
	fi
        for ac_boost_path in $path_list ; do
	    if test -d "$ac_boost_path" && test -r "$ac_boost_path"; then
            	for i in `ls -d $ac_boost_path/include/boost-* 2>/dev/null`; do
		    _version_tmp=`echo $i | sed "s#$ac_boost_path##" | sed 's,/include/boost-,,; s,_,.,'`
                    V_CHECK=`expr $_version_tmp \> $_version`
                    if test "$V_CHECK" = "1" ; then
                        _version=$_version_tmp
                        best_path=$ac_boost_path
		    fi
                done
            fi
	done

        VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
        BOOST_CPPFLAGS="-I$best_path/include/boost-$VERSION_UNDERSCORE"

        if test "$ac_boost_lib_path" = "";  then
	    dnl Look first where we think they ought to be, accounting for a possible "64" suffix on lib.
	    dnl If that directory doesn't exist, fall back to the default behavior
	    if test -d "$best_path/lib${gr_libdir_suffix}"; then
                BOOST_LDFLAGS="-L$best_path/lib${gr_libdir_suffix}"
	    else
                BOOST_LDFLAGS="-L$best_path/lib"
	    fi
        fi

        CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
        export CPPFLAGS
        LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
        export LDFLAGS

        AC_LANG_PUSH(C++)
            AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
        @%:@include <boost/version.hpp>
        ]], [[
        #if BOOST_VERSION >= $WANT_BOOST_VERSION
        // Everything is okay
        #else
        #  error Boost version is too old
        #endif
        ]])],[AC_MSG_RESULT(yes)
	      succeeded=yes
              found_system=yes
              ],
	     [])
        AC_LANG_POP([C++])
        CPPFLAGS="$CPPFLAGS_SAVED"
        LDFLAGS="$LDFLAGS_SAVED"
    fi

    if test "$succeeded" != "yes" ; then
	AC_MSG_RESULT([no])
        if test "$_version" = "0" ; then
            AC_MSG_ERROR([[we could not detect the boost libraries (version $boost_lib_version_req_shorten or higher).
If you are sure you have boost installed, then check your version number looking in <boost/version.hpp>.]])
        else
            AC_MSG_ERROR([your boost libraries seem to old (version $_version).])
        fi
    else
        AC_SUBST(BOOST_CPPFLAGS)
        AC_SUBST(BOOST_LDFLAGS)
        AC_DEFINE(HAVE_BOOST,1,[Define if the Boost headers are available])
    fi
fi
])

dnl
dnl Macros used by the boost items that need libraries.
dnl

dnl $1 is unit name.  E.g., boost_thread
AC_DEFUN([_AX_BOOST_CHECK_LIB],[
    _AX_BOOST_CHECK_LIB_($1,HAVE_[]m4_toupper($1),m4_toupper($1)_LIB)
])

dnl $1 is unit name.  E.g., boost_thread
dnl $2 is AC_DEFINE name.  E.g., HAVE_BOOST_THREAD
dnl $3 is lib var name.    E.g., BOOST_THREAD_LIB
AC_DEFUN([_AX_BOOST_CHECK_LIB_],[
    AC_LANG_PUSH([C++])
    AC_DEFINE($2,1,[Define if the $1 library is available])
    BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

    dnl See if we can find a usable library
    link_ok="no"
    if test "$ax_boost_user_lib" != ""; then
        dnl use what the user supplied 
        for ax_lib in $ax_boost_user_lib $1-${ax_boost_user_lib}; do
	    AC_CHECK_LIB($ax_lib, exit,
                         [$3="-l$ax_lib"; AC_SUBST($3) link_ok="yes"; break])
        done
    else
	dnl Look in BOOSTLIBDIR for possible candidates
	head=$BOOSTLIBDIR/lib[]$1
	for f in ${head}*.so* ${head}*.a* ${head}*.dll* ${head}*.dylib; do
	    dnl echo 1: $f
	    case $f in
	      *\**) continue;;
	    esac
	    f=`echo $f | sed -e 's,.*/,,' -e 's,^lib,,'`
	    dnl echo 2: $f
	    f=`echo $f | sed -e 's,\($1.*\)\.so.*$,\1,' -e 's,\($1.*\)\.a.*$,\1,' -e 's,\($1.*\)\.dll.*$,\1,' -e 's,\($1.*\)\.dylib.*$,\1,'`
	    dnl echo 3: $f

	    ax_lib=$f
            AC_CHECK_LIB($ax_lib, exit,
                        [$3="-l$ax_lib"; AC_SUBST($3) link_ok="yes"; break])
	done
    fi		    
		    		    
    if test "$link_ok" != "yes"; then
    	AC_MSG_ERROR([Could not link against lib[$1]!])
    fi
    AC_LANG_POP([C++])
])


dnl $1 is unit name.  E.g., boost_thread
AC_DEFUN([_AX_BOOST_WITH],[
    _AX_BOOST_WITH_([$1], [m4_bpatsubst($1,_,-)])
])

dnl $1 is unit name.  E.g., boost_thread
dnl $2 is hyphenated unit name.  E.g., boost-thread
AC_DEFUN([_AX_BOOST_WITH_],[
    AC_ARG_WITH([$2],
    		AC_HELP_STRING([--with-$2@<:@=special-lib@:>@],
		               [Use the m4_substr($1,6) library from boost.  It is possible to specify a certain
		                library to the linker.  E.g., --with-$2=$1-gcc41-mt-1_35]),
        	[
	        if test "$withval" = "no"; then
	            want_boost="no"
	        elif test "$withval" = "yes"; then
	            want_boost="yes"
	            ax_boost_user_lib=""
	        else
	            want_boost="yes"
	            ax_boost_user_lib="$withval"
	        fi
	        ],
	        [want_boost="yes"])
])

dnl $1 is unit name.  E.g., boost_thread
dnl $2 is AC_LANG_PROGRAM argument 1
dnl $3 is AC_LANG_PROGRAM argument 2
dnl $4 is cv variable name.  E.g., ax_cv_boost_thread
AC_DEFUN([_AX_BOOST_CHECK_],[
    _AX_BOOST_WITH([$1])
    if test "$want_boost" = "yes"; then
        AC_REQUIRE([AC_PROG_CC])
        AC_REQUIRE([AC_PROG_CXX])
        CPPFLAGS_SAVED="$CPPFLAGS"
        CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
        LDFLAGS_SAVED="$LDFLAGS"
        LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
        AC_CACHE_CHECK([whether the boost::m4_substr([$1],6) includes are available], [$4],
		       [AC_LANG_PUSH([C++])
                        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([$2],[$3])], [[$4]=yes], [[$4]=no])
                        AC_LANG_POP([C++])
			])
	if test "$[$4]" = "yes"; then
	    _AX_BOOST_CHECK_LIB([$1])
	fi
        CPPFLAGS="$CPPFLAGS_SAVED"
        LDFLAGS="$LDFLAGS_SAVED"
    fi
])

dnl $1 is unit name.  E.g., boost_thread
dnl $2 is AC_LANG_PROGRAM argument 1
dnl $3 is AC_LANG_PROGRAM argument 2
AC_DEFUN([_AX_BOOST_CHECK],[
    _AX_BOOST_CHECK_([$1], [$2], [$3], [ax_cv_$1])
])
