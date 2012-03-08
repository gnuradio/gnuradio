#
# SYNOPSIS
#
#   AX_BOOST_TEST_EXEC_MONITOR
#
# DESCRIPTION
#
#   Test for Test_Exec_Monitor library from the Boost C++ libraries. The
#   macro requires a preceding call to AX_BOOST_BASE.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_TEST_EXEC_MONITOR_LIB)
#
#   And sets:
#
#     HAVE_BOOST_TEST_EXEC_MONITOR
#
# COPYLEFT
#
#   Copyright (c) 2008 Dodji Seketeli <dodji@seketeli.org>
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Free Software Foundation, Inc.
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_BOOST_TEST_EXEC_MONITOR],
[
    AC_REQUIRE([AX_BOOST_BASE])
    _AX_BOOST_CHECK([boost_test_exec_monitor],
		    [@%:@include <boost/test/test_tools.hpp>],
                    [int i=1 ; BOOST_REQUIRE(i==1); ; return 0;])
])
