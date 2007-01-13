#
# Copyright 2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

INCLUDES = $(PMT_INCLUDES) $(BOOST_CFLAGS) $(CPPUNIT_INCLUDES)

TESTS = test_mblock

lib_LTLIBRARIES = libmblock.la libmblock-qa.la

EXTRA_DIST = 				


# These are the source files that go into the mblock shared library
libmblock_la_SOURCES =			\
	mb_connection.cc		\
	mb_exception.cc			\
	mb_mblock.cc			\
	mb_mblock_impl.cc		\
	mb_message.cc			\
	mb_port.cc			\
	mb_port_detail.cc		\
	mb_protocol_class.cc		\
	mb_runtime.cc			\
	mb_runtime_impl.cc		\
	mb_util.cc			


# magic flags
libmblock_la_LDFLAGS = $(NO_UNDEFINED) -avoid-version

# link the library against the c++ standard library
libmblock_la_LIBADD = 			\
	$(PMT_LIBS)			\
	-lstdc++			

include_HEADERS =			\
	mb_common.h			\
	mb_exception.h			\
	mb_mblock.h			\
	mb_message.h			\
	mb_port.h			\
	mb_protocol_class.h		\
	mb_runtime.h			\
	mb_util.h			


noinst_HEADERS = 			\
	mb_connection.h			\
	mb_endpoint.h			\
	mb_mblock_impl.h		\
	mb_port_detail.h		\
	mb_runtime_impl.h		\
	qa_mblock.h			\
	qa_mblock_prims.h		


# Build the qa code into its own library

libmblock_qa_la_SOURCES =		\
	qa_mblock.cc			\
	qa_mblock_prims.cc		


# magic flags
libmblock_qa_la_LDFLAGS = $(NO_UNDEFINED) -avoid-version

# link the library against the c++ standard library
libmblock_qa_la_LIBADD = 		\
	libmblock.la			\
	$(CPPUNIT_LIBS)			\
	-lstdc++			


noinst_PROGRAMS	=			\
	test_mblock


LIBMBLOCK   = libmblock.la
LIBMBLOCKQA = libmblock-qa.la $(LIBMBLOCK)

test_mblock_SOURCES = test_mblock.cc
test_mblock_LDADD   = $(LIBMBLOCKQA)


CLEANFILES = $(BUILT_SOURCES) *.pyc
