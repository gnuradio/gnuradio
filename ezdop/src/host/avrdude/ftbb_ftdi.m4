#
# avrdude - A Downloader/Uploader for AVR device programmers
# Copyright (C) 2003-2004  Theodore A. Roth  <troth@openavr.org>
# Copyright (C) 2005 Johnathan Corgan <jcorgan@aeinet.com>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA

AC_DEFUN([FTBB_FTDI],
[
	AC_ARG_ENABLE(
		[ftdi-support],
		AC_HELP_STRING(
			[--enable-ftdi-support],
			[support serial programming via FTDI bit-bang mode (default is no)]),
		[case "${enableval}" in
			yes) ftdi_support=yes ;;
			no)  ftdi_support=no ;;
			*)   AC_MSG_ERROR(bad value ${enableval} for ftdi-support option) ;;
			esac],
		[ftdi_support=no])
	AM_CONDITIONAL(FTDI_SUPPORT, [test "$ftdi_support" = "yes"])

	if test "$ftdi_support" = "yes"; then
		AC_REQUIRE([AC_CANONICAL_HOST])
		AC_LANG_PUSH(C)

		AC_CHECK_HEADERS([ftdi.h],
		  [],
		  [ AC_MSG_ERROR([FTDI support requires libftdi. ftdi.h not found, stop.]) ]
		)

	        save_LIBS="$LIBS"
		AC_SEARCH_LIBS(ftdi_init, [ftdi],
		  [ FTDILIB="$LIBS" ],
		  [ AC_MSG_ERROR([FTDI support requires libftdi. ftdi_init not found, stop.]) ]
		)
	        LIBS="$save_LIBS"

		AC_LANG_POP
		AC_SUBST(FTDILIB)

		AC_DEFINE(FTDI_SUPPORT,[1],[Set to 1 to compile in support for FTDI bit-bang mode programming.])
	fi
])
