#
# Copyright 2010,2011 Free Software Foundation, Inc.
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

#FIXME: forcing the top_builddir for distcheck seems like a bit
# of a hack. Figure out the right way to do this to find built
# volk_config.h and volk_tables.h

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) \
	-I$(top_builddir)/include \
	$(LV_CXXFLAGS) $(WITH_INCLUDES)


# We build 1 library and 1 executable here.  The library contains
# everything except the QA code. The C++ QA code is especially recommended
# when you have general purpose C or C++ code that may not get
# thoroughly exercised by building and running a GR block.  The
# executable runs the QA code at "make check" time.
#
#
#
# N.B., If there's a SWIG generated shared library and associated
# python code, it will be contained in ../python, not here.  (That
# code is conditionally built depending on the state of the
# --without-python configure option.)  However, the .i should be here
# next to the .h that it's based on.


# list of programs run by "make check" and "make distcheck"
#TESTS = testqa
#orc stuff gets built in the ORC directory conditional to ORC being enabled.
#it gets linked in during the build of libvolk as an added library.
#there might be a better way to do this.

lib_LTLIBRARIES = \
	libvolk.la \
	libvolk_runtime.la

EXTRA_DIST = \
	volk_mktables.c		\
	volk_rank_archs.h 	\
	volk_proccpu_sim.c	\
	gcc_x86_cpuid.h

# ----------------------------------------------------------------
#                      The main library
# ----------------------------------------------------------------

libvolk_runtime_la_SOURCES = 	\
	$(platform_CODE) 	\
	volk_runtime.c		\
	volk_init.c 		\
	volk_rank_archs.c	\
	volk_cpu.c

libvolk_la_SOURCES = 		\
	$(platform_CODE) 	\
	volk.c 			\
	volk_environment_init.c

volk_orc_LDFLAGS = \
	$(ORC_LDFLAGS) \
	-lorc-0.4

volk_orc_LIBADD = \
	../orc/libvolk_orc.la

if LV_HAVE_ORC
libvolk_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0 $(volk_orc_LDFLAGS)
libvolk_runtime_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0 $(volk_orc_LDFLAGS)
libvolk_la_LIBADD = $(volk_orc_LIBADD)
else
libvolk_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0
libvolk_runtime_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0
libvolk_la_LIBADD =
endif


# ----------------------------------------------------------------
#        The QA library.  Note libvolk.la in LIBADD
# ----------------------------------------------------------------
#libvolk_qa_la_SOURCES = \
#	qa_utils.cc

#libvolk_qa_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0 -lboost

#libvolk_qa_la_LIBADD = \
#	libvolk.la \
#	libvolk_runtime.la

# ----------------------------------------------------------------
# headers that don't get installed
# ----------------------------------------------------------------
noinst_HEADERS = \
	volk_init.h \
	qa_utils.h

# ----------------------------------------------------------------
# Our test program
# ----------------------------------------------------------------
noinst_PROGRAMS = \
	testqa

testqa_SOURCES = testqa.cc qa_utils.cc
testqa_CPPFLAGS = -DBOOST_TEST_DYN_LINK -DBOOST_TEST_MAIN $(AM_CPPFLAGS)
testqa_LDFLAGS = $(BOOST_UNIT_TEST_FRAMEWORK_LIB)
if LV_HAVE_ORC
testqa_LDADD  = \
	libvolk.la \
	libvolk_runtime.la \
	../orc/libvolk_orc.la
else 
testqa_LDADD  = \
	libvolk.la \
	libvolk_runtime.la
endif

distclean-local: 
	rm -f volk.c
	rm -f volk_cpu_generic.c
	rm -f volk_cpu_powerpc.c
	rm -f volk_cpu_x86.c
	rm -f volk_init.c
	rm -f volk_init.h
	rm -f volk_mktables.c
	rm -f volk_proccpu_sim.c
	rm -f volk_runtime.c
	rm -f volk_tables.h
	rm -f volk_environment_init.c
#SUBDIRS = 

#ifdef BUILD_SSE
#SUBDIRS += sse
#elif BUILD_SPU
#SUBDIRS += spu
#else
#SUBDIRS += port
#endif


