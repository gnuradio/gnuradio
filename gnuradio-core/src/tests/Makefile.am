#
# Copyright 2001,2008,2010 Free Software Foundation, Inc.
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

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(CPPUNIT_INCLUDES) $(WITH_INCLUDES)

# list of programs run by "make check" and "make distcheck"

TESTS = test_all


#	test_atsc		

#Test program to test setting up buffers using gr_test which can be run manually
EXTRA_DIST += \
	test_buffers.py \
	benchmark_dotprod


noinst_PROGRAMS		= 	\
	benchmark_dotprod_fff	\
	benchmark_dotprod_fsf	\
	benchmark_dotprod_fcc	\
	benchmark_dotprod_scc	\
	benchmark_dotprod_ccc	\
	benchmark_dotprod_ccf	\
	benchmark_nco		\
	benchmark_vco		\
	test_all		\
	test_runtime		\
	test_general		\
	test_filter		\
	test_vmcircbuf

#bin_PROGRAMS = \
#	test_all


noinst_SCRIPTS = \
	benchmark_dotprod


LIBGNURADIO = 	$(GNURADIO_CORE_LA) $(BOOST_FILESYSTEM_LIB)
LIBGNURADIOQA = $(top_builddir)/gnuradio-core/src/lib/libgnuradio-core-qa.la $(LIBGNURADIO)

benchmark_dotprod_fff_SOURCES = benchmark_dotprod_fff.cc
benchmark_dotprod_fff_LDADD   = $(LIBGNURADIO)

benchmark_dotprod_fsf_SOURCES = benchmark_dotprod_fsf.cc
benchmark_dotprod_fsf_LDADD   = $(LIBGNURADIO)

benchmark_dotprod_ccf_SOURCES = benchmark_dotprod_ccf.cc
benchmark_dotprod_ccf_LDADD   = $(LIBGNURADIO)

benchmark_dotprod_fcc_SOURCES = benchmark_dotprod_fcc.cc
benchmark_dotprod_fcc_LDADD   = $(LIBGNURADIO)

benchmark_dotprod_scc_SOURCES = benchmark_dotprod_scc.cc
benchmark_dotprod_scc_LDADD   = $(LIBGNURADIO)

benchmark_dotprod_ccc_SOURCES = benchmark_dotprod_ccc.cc
benchmark_dotprod_ccc_LDADD   = $(LIBGNURADIO)

benchmark_nco_SOURCES 	= benchmark_nco.cc
benchmark_nco_LDADD   	= $(LIBGNURADIO)

benchmark_vco_SOURCES 	= benchmark_vco.cc
benchmark_vco_LDADD   	= $(LIBGNURADIO)

test_runtime_SOURCES	= test_runtime.cc
test_runtime_LDADD 	= $(LIBGNURADIOQA)

test_general_SOURCES	= test_general.cc
test_general_LDADD 	= $(LIBGNURADIOQA)

test_filter_SOURCES	= test_filter.cc
test_filter_LDADD 	= $(LIBGNURADIOQA)

# test_atsc_SOURCES	= test_atsc.cc
#test_atsc_LDADD 	= $(LIBGNURADIOQA)

test_vmcircbuf_SOURCES	= test_vmcircbuf.cc
test_vmcircbuf_LDADD 	= $(LIBGNURADIOQA)

test_all_SOURCES	= test_all.cc
test_all_LDADD 		= $(LIBGNURADIOQA)
