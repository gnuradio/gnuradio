#
# SYNOPSIS
#
#   AX_BOOST_SERIALIZATION
#
# DESCRIPTION
#
#   Test for Serialization library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_SERIALIZATION_LIB)
#
#   And sets:
#
#     HAVE_BOOST_SERIALIZATION
#
# COPYLEFT
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Free Software Foundation, Inc.
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_BOOST_SERIALIZATION],
[
    AC_REQUIRE([AX_BOOST_BASE])
    _AX_BOOST_CHECK([boost_serialization],
		    [@%:@include <fstream>
                     @%:@include <boost/archive/text_oarchive.hpp>
                     @%:@include <boost/archive/text_iarchive.hpp>],
                    [std::ofstream ofs("filename");
                     boost::archive::text_oarchive oa(ofs);
                     return 0;])
])
