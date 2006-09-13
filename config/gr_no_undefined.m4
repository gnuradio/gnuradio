dnl
dnl Copyright 2005 Free Software Foundation, Inc.
dnl 
dnl This file is part of GNU Radio
dnl 
dnl GNU Radio is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2, or (at your option)
dnl any later version.
dnl 
dnl GNU Radio is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with GNU Radio; see the file COPYING.  If not, write to
dnl the Free Software Foundation, Inc., 51 Franklin Street,
dnl Boston, MA 02110-1301, USA.
dnl 

# GR_NO_UNDEFINED()
#
# Detemine whether we need to use the -no-undefined linker flag
# when building shared libraries.  
# Sets NO_UNDEFINED to "" or "-no-undefined"
# 
# As far as I can tell, we need -no-undefined only when building
# windows DLLs.  This occurs when using MinGW and Cygwin.
#
# For now, we stub this out.

AC_DEFUN([GR_NO_UNDEFINED],[
	AC_REQUIRE([AC_CANONICAL_HOST])
	no_undefined=""
	case "${host_os}" in
		*mingw* | *cygwin*)

		# on MinGW/Cygwin extra LDFLAGS are required
		no_undefined="-no-undefined"
		;;
	esac
	AC_SUBST(NO_UNDEFINED,[$no_undefined])
])
