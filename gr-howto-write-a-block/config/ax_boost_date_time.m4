#
# SYNOPSIS
#
#   AX_BOOST_DATE_TIME
#
# DESCRIPTION
#
#   Test for date_time library from the Boost C++ libraries.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_DATE_TIME_LIB)
#
#   And sets:
#
#     HAVE_BOOST_DATE_TIME
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

AC_DEFUN([AX_BOOST_DATE_TIME],
[
    AC_REQUIRE([AX_BOOST_BASE])	
    _AX_BOOST_CHECK([boost_date_time],
		    [@%:@include <boost/date_time/gregorian/gregorian_types.hpp>],
		    [using namespace boost::gregorian; date d(2002,Jan,10); return 0;])
])
