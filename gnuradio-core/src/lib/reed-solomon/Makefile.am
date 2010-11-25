#
# Copyright 2002,2008 Free Software Foundation, Inc.
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
# 

include $(top_srcdir)/Makefile.common

# Note, this Makefile.am only builds the char versions of the reed soloman routines.
# If you need the int versions too, please figure out how to add them *cleanly* to 
# this Makefile.am.

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(CPPUNIT_INCLUDES) $(WITH_INCLUDES)

TESTS = rstest

EXTRA_DIST +=			\
	README.karn

noinst_LTLIBRARIES = librs.la

noinst_PROGRAMS = 		\
	rstest

librs_la_SOURCES = 		\
	encode_rs.c		\
	decode_rs.c		\
	init_rs.c

grinclude_HEADERS = 		\
	rs.h

noinst_HEADERS = 		\
	ccsds.h			\
	char.h			\
	int.h			\
	fixed.h			

rstest_SOURCES =	rstest.c exercise.c
rstest_LDADD =		librs.la
