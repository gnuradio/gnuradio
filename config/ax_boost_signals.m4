#
# SYNOPSIS
#
#   AX_BOOST_SIGNALS
#
# DESCRIPTION
#
#   Test for Signals library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_SIGNALS_LIB)
#
#   And sets:
#
#     HAVE_BOOST_SIGNALS
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

AC_DEFUN([AX_BOOST_SIGNALS],
[
    AC_REQUIRE([AX_BOOST_BASE])
    _AX_BOOST_CHECK([boost_signals],
		    [@%:@include <boost/signal.hpp>],
                    [boost::signal<void ()> sig; return 0;])
])
