#
# Copyright 2007,2008,2010 Free Software Foundation, Inc.
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

IBM_PPU_SYNC_INCLUDES = -I$(top_srcdir)/gcell/ibm/sync/ppu_source


AM_CPPFLAGS = $(DEFINES) $(BOOST_CPPFLAGS) $(CPPUNIT_INCLUDES) \
	$(GCELL_INCLUDES) $(IBM_PPU_SYNC_INCLUDES) $(WITH_INCLUDES)


dist_bin_SCRIPTS = gcell-embedspu-libtool

noinst_LTLIBRARIES = libruntime.la libruntime-qa.la

libruntime_la_SOURCES = \
	gc_aligned_alloc.cc \
	gc_job_manager.cc \
	gc_job_manager_impl.cc \
	gc_jd_queue.c \
	gc_jd_stack.c \
	gc_proc_def_utils.cc

libruntime_qa_la_SOURCES = \
	qa_gcell_runtime.cc \
	qa_jd_queue.cc \
	qa_jd_stack.cc \
	qa_job_manager.cc


noinst_HEADERS = \
	gc_client_thread_info.h \
	gc_job_manager_impl.h \
	gc_proc_def_utils.h \
	qa_jd_queue.h \
	qa_jd_stack.h \
	qa_job_manager.h \
	qa_gcell_runtime.h

# generate a libtool.lo that contains an embeded SPU executable
gcell_runtime_qa.lo: ../spu/gcell_runtime_qa
	$(GCELL_EMBEDSPU_LIBTOOL) $< $@

libruntime_qa_la_LIBADD = \
	gcell_runtime_qa.lo \
	libruntime.la

CLEANFILES = gcell_runtime_qa.lo
