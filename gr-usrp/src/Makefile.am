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
EXTRA_DIST += run_tests.in run_guile_tests.in $(nobase_guile_DATA)
DISTCLEANFILES += run_tests run_guile_tests

noinst_PYTHON =	qa_usrp.py
noinst_GUILE  = usrp.test

if GUILE
nobase_guile_DATA = gnuradio/usrp.scm
endif

# ----------------------------------------------------------------
# The straight C++ library

AM_CPPFLAGS = \
	$(USRP_INCLUDES) \
	$(STD_DEFINES_AND_INCLUDES) \
	$(PYTHON_CPPFLAGS) \
	$(USB_INCLUDES) \
	$(WITH_INCLUDES)

lib_LTLIBRARIES = \
	libgnuradio-usrp.la

libgnuradio_usrp_la_SOURCES = \
	usrp_base.cc \
	usrp_sink_base.cc \
	usrp_sink_c.cc \
	usrp_sink_s.cc \
	usrp_source_base.cc \
	usrp_source_c.cc \
	usrp_source_s.cc

libgnuradio_usrp_la_LIBADD = \
	$(GNURADIO_CORE_LA) \
	$(USRP_LA)			

libgnuradio_usrp_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)

grinclude_HEADERS = \
	usrp_base.h \
	usrp_sink_base.h \
	usrp_sink_c.h \
	usrp_sink_s.h \
	usrp_source_base.h \
	usrp_source_c.h \
	usrp_source_s.h

# ----------------------------------------------------------------
# The SWIG library

TOP_SWIG_IFILES =		\
	usrp_swig.i

# Install so that they end up available as:
#   import gnuradio.usrp
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/usrp
usrp_swig_pythondir_category = 	\
	gnuradio/usrp

# additional arguments to the SWIG command
usrp_swig_swig_args =		\
	$(USRP_INCLUDES)

# additional libraries for linking with the SWIG-generated library
usrp_swig_la_swig_libadd =	\
	libgnuradio-usrp.la

# additional Python files to be installed along with the SWIG-generated one
usrp_swig_python =		\
	__init__.py

# additional SWIG files to be installed
usrp_swig_swiginclude_headers =	\
	usrp_base.i		\
	usrp_source_base.i	\
	usrp_source_c.i		\
	usrp_source_s.i		\
	usrp_sink_base.i	\
	usrp_sink_c.i		\
	usrp_sink_s.i		\
	usrp_standard.i


if PYTHON
TESTS += run_tests
endif

if GUILE
TESTS += run_guile_tests
endif

