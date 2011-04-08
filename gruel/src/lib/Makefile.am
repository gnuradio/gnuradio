#
# Copyright 2008,2009,2010,2011 Free Software Foundation, Inc.
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

SUBDIRS = pmt msg

AM_CPPFLAGS = $(DEFINES) $(GRUEL_INCLUDES) $(BOOST_CPPFLAGS) \
	$(CPPUNIT_INCLUDES) $(WITH_INCLUDES)

TESTS = test_gruel

noinst_PROGRAMS = test_gruel


lib_LTLIBRARIES = libgruel.la

# magic flags
libgruel_la_LDFLAGS = $(NO_UNDEFINED) $(BOOST_LDFLAGS) $(LTVERSIONFLAGS)

# ----------------------------------------------------------------

PMT_LIB = pmt/libpmt.la
MSG_LIB = msg/libmsg.la

# These are the source files that go into the gruel shared library
libgruel_la_SOURCES = 			\
	realtime.cc 			\
	sys_pri.cc 			\
	thread_body_wrapper.cc 		\
	thread_group.cc

libgruel_la_LIBADD = 			\
	$(BOOST_THREAD_LIB) 		\
	$(BOOST_SYSTEM_LIB) 		\
	$(BOOST_FILESYSTEM_LIB) 	\
	$(PMT_LIB)			\
	$(MSG_LIB)			\
	-lstdc++


# ----------------------------------------------------------------

test_gruel_SOURCES = test_gruel.cc
test_gruel_LDADD   =	\
	$(BOOST_THREAD_LIB) 		\
	$(BOOST_SYSTEM_LIB) 		\
	$(BOOST_FILESYSTEM_LIB) 	\
	pmt/libpmt-qa.la libgruel.la

