#
# Copyright 2003,2004,2008,2009,2011 Free Software Foundation, Inc.
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

AM_CPPFLAGS = $(GRUEL_INCLUDES) $(GNURADIO_INCLUDES) $(WITH_INCLUDES)

EXTRA_DIST += 			\
	getopt.h 		\
	getopt.c 		\
	gettimeofday.c 		\
	posix_memalign.cc	\
	posix_memalign.h	\
	usleep.c		

noinst_LTLIBRARIES = libmissing.la

libmissing_la_common_SOURCES =	\
	bug_work_around_8.cc

powerpc_CODE = \
	posix_memalign.cc	

if MD_CPU_powerpc
libmissing_la_SOURCES = $(libmissing_la_common_SOURCES) $(powerpc_CODE)
else
libmissing_la_SOURCES = $(libmissing_la_common_SOURCES)
endif
