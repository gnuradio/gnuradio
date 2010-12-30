dnl 
dnl  Copyright 2004,2005,2007,2008,2009 Free Software Foundation, Inc.
dnl  
dnl  This file is part of GNU Radio
dnl  
dnl  GNU Radio is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU General Public License as published by
dnl  the Free Software Foundation; either version 3, or (at your option)
dnl  any later version.
dnl  
dnl  GNU Radio is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl  GNU General Public License for more details.
dnl  
dnl  You should have received a copy of the GNU General Public License
dnl  along with GNU Radio; see the file COPYING.  If not, write to
dnl  the Free Software Foundation, Inc., 51 Franklin Street,
dnl  Boston, MA 02110-1301, USA.
dnl 

AC_INIT
AC_PREREQ(2.57)
AC_CONFIG_AUX_DIR([.])	

AC_CONFIG_HEADERS([grconfig.h])
AC_CONFIG_MACRO_DIR(config)
AC_CONFIG_SRCDIR(gr-run-waveform-binary.cc)
AM_INIT_AUTOMAKE("gr-run-waveform", "0.1")

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
dnl AC_CANONICAL_TARGET

dnl Find the the C++ compiler
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LN_S

GR_LIB64		dnl check for lib64 suffix after choosing compilers

dnl Checks for standard header files.
AC_HEADER_STDC

dnl Get the paths to the Guile headers and library
GUILE_DEVEL

AC_PATH_PROG(PYTHON, python)
AC_SUBST(PYTHON)

AC_CONFIG_FILES([\
	  Makefile \
	  config/Makefile \
	])

AC_OUTPUT

echo "Configured gr-run-waveform release $RELEASE for build."
