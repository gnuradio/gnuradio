#
# SYNOPSIS
#
#   AX_BOOST_FILESYSTEM
#
# DESCRIPTION
#
#   Test for Filesystem library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. 
#
#   This macro calls:
#
#     AC_SUBST(BOOST_FILESYSTEM_LIB)
#
#   And sets:
#
#     HAVE_BOOST_FILESYSTEM
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

AC_DEFUN([AX_BOOST_FILESYSTEM],
[
    AC_REQUIRE([AX_BOOST_BASE])

    dnl depends on boost_system
    AC_REQUIRE([AX_BOOST_SYSTEM])
    axbf_LDFLAGS_SAVED=$LDFLAGS
    LDFLAGS="$LDFLAGS $BOOST_SYSTEM_LIB"

    _AX_BOOST_CHECK([boost_filesystem],
		    [@%:@include <boost/filesystem/path.hpp>],
		    [using namespace boost::filesystem;
                     path my_path( "foo/bar/data.txt" );
                     return 0;])

   LDFLAGS=$axbf_LDFLAGS_SAVED
])
