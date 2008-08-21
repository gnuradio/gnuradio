#
# SYNOPSIS
#
#   AX_BOOST_THREAD
#
# DESCRIPTION
#
#   Test for Thread library from the Boost C++ libraries.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_THREAD_LIB)
#     AC_SUBST(BOOST_CXXFLAGS)
#
#   And sets:
#
#     HAVE_BOOST_THREAD
#
# COPYLEFT
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Michael Tindal
#   Copyright (c) 2008 Free Software Foundation, Inc.
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.


AC_DEFUN([AX_BOOST_THREAD],
[
    AC_REQUIRE([AX_BOOST_BASE])
    AC_REQUIRE([ACX_PTHREAD])
    _AX_BOOST_WITH([boost_thread])

    if test "$want_boost" = "yes"; then
        AC_REQUIRE([AC_PROG_CC])
        AC_REQUIRE([AC_PROG_CXX])
        AC_REQUIRE([AC_CANONICAL_HOST])

        CPPFLAGS_SAVED="$CPPFLAGS"
        LDFLAGS_SAVED="$LDFLAGS"
	CXXFLAGS_SAVED="$CXXFLAGS"

        CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
        LDFLAGS="$LDFLAGS $BOOST_LDFLAGS $PTHREAD_LIBS"
	CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"

        AC_CACHE_CHECK(whether the boost::thread includes are available,
                       ax_cv_boost_thread,
        [AC_LANG_PUSH([C++])
             AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[@%:@include <boost/thread/thread.hpp>]],
                                   [[boost::thread_group thrds;
                                   return 0;]]),
                   ax_cv_boost_thread=yes, ax_cv_boost_thread=no)
             AC_LANG_POP([C++])
        ])

        if test "$ax_cv_boost_thread" = "yes"; then
	    BOOST_CXXFLAGS="$PTHREAD_CFLAGS"
            AC_SUBST(BOOST_CXXFLAGS)
	    _AX_BOOST_CHECK_LIB([boost_thread])
	    if test "$link_ok" = "yes" && test -n "$PTHREAD_LIBS"; then
	        BOOST_THREAD_LIB="$BOOST_THREAD_LIB $PTHREAD_LIBS"
	    fi
        fi

        CPPFLAGS="$CPPFLAGS_SAVED"
        LDFLAGS="$LDFLAGS_SAVED"
	CXXFLAGS="$CXXFLAGS_SAVED"
    fi
])
