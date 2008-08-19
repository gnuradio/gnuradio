#
# Copyright 2006,2007,2008 Free Software Foundation, Inc.
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

AM_CPPFLAGS = $(DEFINES) $(OMNITHREAD_INCLUDES) $(PMT_INCLUDES) \
	$(BOOST_CPPFLAGS) $(CPPUNIT_INCLUDES) $(WITH_INCLUDES)

# disable test until we fix ticket:180
# TESTS = test_mblock

lib_LTLIBRARIES = libmblock.la libmblock-qa.la

EXTRA_DIST = 				\
	README.locking			\
	qa_bitset.mbh			


BUILT_SOURCES =				\
	qa_bitset_mbh.cc		

qa_bitset_mbh.cc : qa_bitset.mbh
	$(COMPILE_MBH) $(srcdir)/qa_bitset.mbh qa_bitset_mbh.cc

# These are the source files that go into the mblock shared library
libmblock_la_SOURCES =			\
	mb_class_registry.cc		\
	mb_connection.cc		\
	mb_endpoint.cc			\
	mb_exception.cc			\
	mb_gettid.cc			\
	mb_mblock.cc			\
	mb_mblock_impl.cc		\
	mb_message.cc			\
	mb_msg_accepter.cc		\
	mb_msg_accepter_msgq.cc		\
	mb_msg_accepter_smp.cc		\
	mb_msg_queue.cc			\
	mb_port.cc			\
	mb_port_simple.cc		\
	mb_protocol_class.cc		\
	mb_runtime.cc			\
	mb_runtime_base.cc		\
	mb_runtime_nop.cc		\
	mb_runtime_thread_per_block.cc	\
	mb_timer_queue.cc		\
	mb_util.cc			\
	mb_worker.cc			


# magic flags
libmblock_la_LDFLAGS = $(NO_UNDEFINED)

# link the library against the c++ standard library
libmblock_la_LIBADD = 			\
	$(OMNITHREAD_LA)		\
	$(PMT_LA)			\
	-lstdc++			

include_HEADERS =			\
	mb_class_registry.h		\
	mb_common.h			\
	mb_exception.h			\
	mb_gettid.h			\
	mb_mblock.h			\
	mb_message.h			\
	mb_msg_accepter.h		\
	mb_msg_accepter_msgq.h		\
	mb_msg_queue.h			\
	mb_port.h			\
	mb_port_simple.h		\
	mb_protocol_class.h		\
	mb_runtime.h			\
	mb_time.h			\
	mb_util.h			


noinst_HEADERS = 			\
	mb_connection.h			\
	mb_endpoint.h			\
	mb_mblock_impl.h		\
	mb_msg_accepter_smp.h		\
	mb_runtime_base.h		\
	mb_runtime_nop.h		\
	mb_runtime_thread_per_block.h	\
	mb_timer_queue.h		\
	mb_worker.h			\
	mbi_runtime_lock.h		\
	qa_mblock.h			\
	qa_mblock_prims.h		\
	qa_mblock_send.h		\
	qa_mblock_sys.h			\
	qa_timeouts.h			


# Build the qa code into its own library

libmblock_qa_la_SOURCES =		\
	qa_bitset.cc			\
	qa_bitset_mbh.cc		\
	qa_disconnect.cc		\
	qa_mblock.cc			\
	qa_mblock_prims.cc		\
	qa_mblock_send.cc		\
	qa_mblock_sys.cc		\
	qa_timeouts.cc			


# magic flags
libmblock_qa_la_LDFLAGS = $(NO_UNDEFINED) -avoid-version

# link the library against the c++ standard library
libmblock_qa_la_LIBADD = 		\
	libmblock.la			\
	$(CPPUNIT_LIBS)			\
	-lstdc++			


noinst_PROGRAMS	= 			\
	test_mblock			\
	benchmark_send			

test_mblock_SOURCES = test_mblock.cc
test_mblock_LDADD   = libmblock-qa.la

benchmark_send_SOURCES = benchmark_send.cc
benchmark_send_LDADD   = libmblock-qa.la

CLEANFILES = $(BUILT_SOURCES) *.pyc
