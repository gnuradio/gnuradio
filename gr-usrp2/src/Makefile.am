#
# Copyright 2004,2005,2006,2008,2009,2010 Free Software Foundation, Inc.
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
include $(top_srcdir)/Makefile.swig

TESTS =
EXTRA_DIST += run_tests.in run_guile_tests.in
DISTCLEANFILES += run_tests run_guile_tests

noinst_PYTHON = qa_usrp2.py

if GUILE
nobase_guile_DATA = gnuradio/usrp2.scm
endif

# ----------------------------------------------------------------------
# C++ block API interface librar(ies)
#
# libgr-usrp.so
# ----------------------------------------------------------------------
AM_CPPFLAGS = \
	$(GRUEL_INCLUDES) \
	$(USRP2_INCLUDES) \
	$(STD_DEFINES_AND_INCLUDES)  \
	$(PYTHON_CPPFLAGS) \
	$(WITH_INCLUDES)

lib_LTLIBRARIES = libgnuradio-usrp2.la

libgnuradio_usrp2_la_LDFLAGS = $(LTVERSIONFLAGS)

libgnuradio_usrp2_la_SOURCES = \
	rx_16sc_handler.cc \
	rx_32fc_handler.cc \
	usrp2_base.cc \
	usrp2_source_base.cc \
	usrp2_source_16sc.cc \
	usrp2_source_32fc.cc \
	usrp2_sink_base.cc \
	usrp2_sink_16sc.cc \
	usrp2_sink_32fc.cc

libgnuradio_usrp2_la_LIBADD = \
	$(USRP2_LA) \
	$(GNURADIO_CORE_LA) 

grinclude_HEADERS = \
	usrp2_base.h \
	usrp2_source_base.h \
	usrp2_source_32fc.h \
	usrp2_source_16sc.h \
	usrp2_sink_base.h \
	usrp2_sink_16sc.h \
	usrp2_sink_32fc.h

noinst_HEADERS = \
	rx_16sc_handler.h \
	rx_32fc_handler.h

# ----------------------------------------------------------------------
# SWIG wrappers around C++ library
#
# usrp2.py
# _usrp2.so
# ----------------------------------------------------------------------

TOP_SWIG_IFILES =		\
	usrp2_swig.i

# Install so that they end up available as:
#   import gnuradio.usrp2
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/usrp2
usrp2_swig_pythondir_category =	\
	gnuradio/usrp2

# additional arguments to the SWIG command
usrp2_swig_swig_args =		\
	$(USRP2_INCLUDES)

# additional libraries for linking with the SWIG-generated library
usrp2_swig_la_swig_libadd =	\
	libgnuradio-usrp2.la

# additional Python files to be installed along with the SWIG-generated one
usrp2_swig_python =		\
	__init__.py

if PYTHON
TESTS += run_tests
endif

if GUILE
TESTS += run_guile_tests
endif
