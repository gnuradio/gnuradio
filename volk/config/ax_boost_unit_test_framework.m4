#
# SYNOPSIS
#
#   AX_BOOST_UNIT_TEST_FRAMEWORK
#
# DESCRIPTION
#
#   Test for Unit_Test_Framework library from the Boost C++ libraries. The
#   macro requires a preceding call to AX_BOOST_BASE.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_UNIT_TEST_FRAMEWORK_LIB)
#
#   And sets:
#
#     HAVE_BOOST_UNIT_TEST_FRAMEWORK
#
# COPYLEFT
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Free Software Foundation, Inc.
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_BOOST_UNIT_TEST_FRAMEWORK],
[
    AC_REQUIRE([AX_BOOST_BASE])
    _AX_BOOST_CHECK([boost_unit_test_framework],
		    [@%:@include <boost/test/unit_test.hpp>],
		    [using boost::unit_test::test_suite;
                     test_suite* test= BOOST_TEST_SUITE( "Unit test example 1" );
                     return 0;])
])
