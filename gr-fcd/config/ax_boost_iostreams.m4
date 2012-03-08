#
# SYNOPSIS
#
#   AX_BOOST_IOSTREAMS
#
# DESCRIPTION
#
#   Test for IOStreams library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_IOSTREAMS_LIB)
#
#   And sets:
#
#     HAVE_BOOST_IOSTREAMS
#
# COPYLEFT
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Free Software Foundation, Inc.
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_BOOST_IOSTREAMS],
[
    AC_REQUIRE([AX_BOOST_BASE])
    _AX_BOOST_CHECK([boost_iostreams],
                    [@%:@include <boost/iostreams/filtering_stream.hpp>
                     @%:@include <boost/range/iterator_range.hpp>],
                    [std::string  input = "Hello World!";
                     namespace io = boost::iostreams;
                     io::filtering_istream  in(boost::make_iterator_range(input));
                     return 0;])

])
