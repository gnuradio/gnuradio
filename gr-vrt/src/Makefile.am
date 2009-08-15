#
# Copyright 2004,2005,2006,2008,2009 Free Software Foundation, Inc.
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

# ----------------------------------------------------------------------
# Local Python files, not installed
#
# qa_vrt.py
# ----------------------------------------------------------------------

noinst_PYTHON = qa_vrt.py

# ----------------------------------------------------------------------
# Miscellaneous build operations
# ----------------------------------------------------------------------

EXTRA_DIST = run_tests.in
TESTS = run_tests
DISTCLEANFILES = run_tests

# ----------------------------------------------------------------------
# C++ block API interface librar(ies)
#
# libgnuradio_vrt.so
# ----------------------------------------------------------------------
AM_CPPFLAGS = \
	$(STD_DEFINES_AND_INCLUDES)  \
	$(GRUEL_INCLUDES) \
	$(PYTHON_CPPFLAGS) \
	$(VRT_INCLUDES) \
	$(WITH_INCLUDES)

lib_LTLIBRARIES = libgnuradio-vrt.la

libgnuradio_vrt_la_SOURCES = \
	missing_pkt_checker.cc \
	vrt_source_base.cc \
	vrt_source_32fc.cc \
	vrt_quadradio_source_32fc.cc


#libgnuradio_vrt_la_SOURCES = \
#	rx_16sc_handler.cc \
#	rx_32fc_handler.cc \
#	vrt_base.cc \
#	vrt_source_base.cc \
#	vrt_source_16sc.cc \
#	vrt_source_32fc.cc

#	vrt_sink_base.cc \
#	vrt_sink_16sc.cc \
#	vrt_sink_32fc.cc

libgnuradio_vrt_la_LIBADD = \
	$(VRT_LA) \
	$(GNURADIO_CORE_LA) 

grinclude_HEADERS = \
	vrt_source_base.h \
	vrt_source_32fc.h \
	vrt_quadradio_source_32fc.h

#	vrt_source_16sc.h \
#	vrt_sink_base.h \
#	vrt_sink_16sc.h \
#	vrt_sink_32fc.h

noinst_HEADERS = \
	missing_pkt_checker.h


# ----------------------------------------------------------------------
# Python SWIG wrapper around C++ library
#
# vrt.py
# _vrt.so
# ----------------------------------------------------------------------

TOP_SWIG_IFILES =		\
	vrt.i

# Install so that they end up available as:
#   import gnuradio.vrt
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio
vrt_pythondir_category =	\
	gnuradio

# additional arguments to the SWIG command
vrt_swig_args =		\
	$(VRT_INCLUDES)

# additional libraries for linking with the SWIG-generated library
vrt_la_swig_libadd =		\
	libgnuradio-vrt.la

include $(top_srcdir)/Makefile.swig

# add some of the variables generated inside the Makefile.swig.gen
BUILT_SOURCES = $(swig_built_sources)

# Do not distribute the output of SWIG
no_dist_files = $(swig_built_sources)
