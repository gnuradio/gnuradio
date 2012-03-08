# Check for (MinGW)win32 extra ld options.             -*- Autoconf -*-

# Copyright 2003,2004,2005 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

dnl
AC_DEFUN([GR_LIBGNURADIO_CORE_EXTRA_LDFLAGS], [
AC_REQUIRE([AC_PROG_LD])
# on Mingw32 extra LDFLAGS are required to ease global variable linking
LIBGNURADIO_CORE_EXTRA_LDFLAGS=""

AC_MSG_CHECKING([whether $LD accepts --enable-runtime-pseudo-reloc])
if ${LD} --enable-runtime-pseudo-reloc --version >/dev/null 2>&1
then
        # libtool requires the quotes
        LIBGNURADIO_CORE_EXTRA_LDFLAGS="\"-Wl,--enable-runtime-pseudo-reloc\""
        AC_MSG_RESULT(yes)
else
        AC_MSG_RESULT(no)
fi

AC_SUBST(LIBGNURADIO_CORE_EXTRA_LDFLAGS)

])
