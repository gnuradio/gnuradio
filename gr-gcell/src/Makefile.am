#
# Copyright 2008,2009,2010 Free Software Foundation, Inc.
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

SUBDIRS = . examples

AM_CPPFLAGS = $(GCELL_INCLUDES) $(STD_DEFINES_AND_INCLUDES) \
	$(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

# ----------------------------------------------------------------

noinst_PYTHON =  \
	qa_fft.py

DISTCLEANFILES = \
	gcell.d \
	run_tests

# ----------------------------------------------------------------
#                        The C++ blocks
# ----------------------------------------------------------------

# the library for the C++ blocks
lib_LTLIBRARIES = libgnuradio_gcell.la

libgnuradio_gcell_la_SOURCES = \
	gcell_fft_vcc.cc

# C/C++ headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS = \
	gcell_fft_vcc.h

libgnuradio_gcell_la_LIBADD = \
	$(GNURADIO_CORE_LA) \
	$(GCELL_LA)

libgnuradio_gcell_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)

if PYTHON
# ----------------------------------------------------------------
#                         SWIG stuff
# ----------------------------------------------------------------
EXTRA_DIST = run_tests.in
TESTS = run_tests

TOP_SWIG_IFILES =		\
	gcell.i

# Install so that they end up available as:
#   import gnuradio.gcell
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio
gcell_pythondir_category =	\
	gnuradio

# additional arguments to the SWIG command
gcell_swig_args =		\
	$(GCELL_INCLUDES)

# additional libraries for linking with the SWIG-generated library
gcell_la_swig_libadd =		\
	libgnuradio_gcell.la

# additional SWIG files to be installed
gcell_swiginclude_headers =	\
	gc_job_manager.i	\
	gcell_fft_vcc.i

include $(top_srcdir)/Makefile.swig

# add some of the variables generated inside the Makefile.swig.gen
BUILT_SOURCES = $(swig_built_sources)

# Do not distribute the output of SWIG
no_dist_files = $(swig_built_sources)
endif
