#
# Copyright 2008 Free Software Foundation, Inc.
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

# We're building a single .a file from files in several
# subdirectories.  We use the "single Makefile, multiple subdirectory"
# automake alternative. We're doing this because we're faking out
# automake and getting it to build for 2 architectures at the same
# time, the PPE (powerpc64) and the SPE.  The easiest way to handle
# the SPE was to just build a static library using automake's built in
# rules, since trying to get libtool to handle two architectures in
# the same tree seemed untenable.

include $(top_srcdir)/Makefile.common.spu

IBM_SPU_SYNC_INCLUDES = -I$(top_srcdir)/gcell/src/ibm/sync/spu_source
AM_CPPFLAGS = $(GCELL_SPU_INCLUDES) $(IBM_SPU_SYNC_INCLUDES)

lib_LIBRARIES = libgcell_spu.a

# ----------------------------------------------------------------
# files in the lib/runtime/spu directory

runtime_srcdir = $(srcdir)/../runtime/spu

runtime_spu_sources = \
	$(runtime_srcdir)/gc_delay.c \
	$(runtime_srcdir)/gc_spu_jd_queue.c \
	$(runtime_srcdir)/spu_buffers.c \
	$(runtime_srcdir)/gc_logging.c \
	$(runtime_srcdir)/gc_main.c \
	$(runtime_srcdir)/gc_random.c


runtime_spu_headers =

runtime_spu_noinst_headers = \
	$(runtime_srcdir)/gc_spu_config.h \
	$(runtime_srcdir)/spu_buffers.h 

# ----------------------------------------------------------------
# files in the lib/general/spu directory

general_srcdir = $(srcdir)/../general/spu

general_spu_sources = \
	$(general_srcdir)/fft_1d_r2.c \
	$(general_srcdir)/memset.S

general_spu_headers = \
	$(general_srcdir)/libfft.h

general_spu_noinst_headers = \
	$(general_srcdir)/fft_1d.h \
	$(general_srcdir)/fft_1d_r2.h

# The QA code for (usually) non-PPE visible support routines in lib/general/spu
general_spu_qa_sources = \
	$(general_srcdir)/qa_memset.c

# ----------------------------------------------------------------
# files in the lib/wrapper/spu directory

wrapper_srcdir = $(srcdir)/../wrapper/spu

wrapper_spu_sources = \
	$(wrapper_srcdir)/gcs_fft_1d_r2.c

wrapper_spu_headers =

wrapper_spu_noinst_headers =

# ----------------------------------------------------------------
# build the library from the files in the three directories

libgcell_spu_a_SOURCES = \
	$(runtime_spu_sources) \
	$(general_spu_sources) \
	$(wrapper_spu_sources)

gcellspuinclude_HEADERS = \
	$(runtime_spu_headers) \
	$(general_spu_headers) \
	$(wrapper_spu_headers)

noinst_HEADERS = \
	$(runtime_spu_noinst_headers) \
	$(general_spu_noinst_headers) \
	$(wrapper_spu_noinst_headers)

# ----------------------------------------------------------------
# build some SPU executables

noinst_PROGRAMS = \
	gcell_all \
	gcell_runtime_qa \
	gcell_general_qa

#
# All known non-QA gcell procs (at least until they get too big).
#
gcell_all_SOURCES = $(wrapper_spu_sources)
gcell_all_LDADD = libgcell_spu.a

#
# The QA code required for testing the runtime.
#
gcell_runtime_qa_SOURCES = $(runtime_srcdir)/gcell_runtime_qa.c
gcell_runtime_qa_LDADD = libgcell_spu.a

#
# The QA code required for testing the SPE support routines in lib/general/spu
#
gcell_general_qa_SOURCES = $(general_spu_qa_sources)
gcell_general_qa_LDADD = libgcell_spu.a
