#
# Copyright 2007,2008 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

include $(top_srcdir)/Makefile.common

SUBDIRS = spu .

AM_CPPFLAGS = $(DEFINES) $(OMNITHREAD_INCLUDES) \
	$(GCELL_INCLUDES) $(CPPUNIT_INCLUDES) $(WITH_INCLUDES)


GCELL_QA_LA = $(top_builddir)/gcell/lib/libgcell-qa.la

TESTS = test_all


bin_PROGRAMS = \
	test_all \
	benchmark_dma \
	benchmark_nop


test_all_SOURCES = test_all.cc
test_all_LDADD = $(GCELL_QA_LA) $(GCELL_LA)

benchmark_dma_SOURCES = benchmark_dma.cc
benchmark_dma_LDADD = spu/benchmark_procs $(GCELL_LA)

benchmark_nop_SOURCES = benchmark_nop.cc
benchmark_nop_LDADD = spu/benchmark_procs $(GCELL_LA)
