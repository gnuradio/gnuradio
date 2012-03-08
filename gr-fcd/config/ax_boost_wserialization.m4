#
# SYNOPSIS
#
#   AX_BOOST_WSERIALIZATION
#
# DESCRIPTION
#
#   Test for WSerialization library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_WSERIALIZATION_LIB)
#
#   And sets:
#
#     HAVE_BOOST_WSERIALIZATION
#
# COPYLEFT
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Free Software Foundation, Inc.
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_BOOST_WSERIALIZATION],
[
    AC_REQUIRE([AX_BOOST_BASE])

    dnl depends on BOOST_SERIALIZATION
    AC_REQUIRE([AX_BOOST_SERIALIZATION])
    axbws_LDFLAGS_SAVED=$LDFLAGS
    LDFLAGS="$LDFLAGS $BOOST_SERIALIZATION_LIB"

    _AX_BOOST_CHECK([boost_wserialization],
		    [@%:@include <fstream>
                     @%:@include <boost/archive/text_oarchive.hpp>
                     @%:@include <boost/archive/text_iarchive.hpp>],
                    [std::ofstream ofs("filename");
                     boost::archive::text_oarchive oa(ofs);
                     return 0;])

    LDFLAGS=$axbf_LDFLAGS_SAVED
])
