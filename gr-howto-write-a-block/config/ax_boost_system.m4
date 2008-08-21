# ===========================================================================
# started with this: http://autoconf-archive.cryp.to/ax_boost_system.html,
# virtually nothing left
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_SYSTEM
#
# DESCRIPTION
#
#   Test for System library from the Boost C++ libraries. The macro requires
#   a preceding call to AX_BOOST_BASE.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_SYSTEM_LIB)
#
#   And sets:
#
#     HAVE_BOOST_SYSTEM
#
# COPYLEFT
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Michael Tindal
#   Copyright (c) 2008 Daniel Casimiro <dan.casimiro@gmail.com>
#   Copyright (c) 2008 Free Software Foundation, Inc.
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_BOOST_SYSTEM],
[
    AC_REQUIRE([AX_BOOST_BASE])	
    _AX_BOOST_CHECK([boost_system],
		    [@%:@include <boost/system/error_code.hpp>],
		    [boost::system::system_category])
])
