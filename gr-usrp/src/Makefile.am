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

# ----------------------------------------------------------------
# The straight C++ library

AM_CPPFLAGS = \
	$(STD_DEFINES_AND_INCLUDES) \
	$(PYTHON_CPPFLAGS) \
	$(USRP_INCLUDES) \
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

libgnuradio_usrp_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0

grinclude_HEADERS = \
	usrp_base.h \
	usrp_sink_base.h \
	usrp_sink_c.h \
	usrp_sink_s.h \
	usrp_source_base.h \
	usrp_source_c.h \
	usrp_source_s.h

# ----------------------------------------------------------------
# The SWIG library and Python modules
#
# Install this stuff so that it ends up as the gnuradio.usrp module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/usrp

ourpythondir = $(grpythondir)/usrp
ourlibdir    = $(grpyexecdir)/usrp

BUILT_SOURCES = \
	usrp_swig.cc \
	usrp_swig.py		

LOCAL_IFILES = \
	$(srcdir)/usrp.i \
	$(srcdir)/usrp_base.i \
	$(srcdir)/usrp_source_base.i \
	$(srcdir)/usrp_source_c.i \
	$(srcdir)/usrp_source_s.i \
	$(srcdir)/usrp_sink_base.i \
	$(srcdir)/usrp_sink_c.i \
	$(srcdir)/usrp_sink_s.i \
	$(srcdir)/usrp_standard.i

NON_LOCAL_IFILES = \
	$(GNURADIO_I)

ALL_IFILES = \
	$(LOCAL_IFILES)	\
	$(NON_LOCAL_IFILES)		

ourlib_LTLIBRARIES = \
	_usrp_swig.la

ourlib_PYTHON = \
	__init__.py \
	usrp_swig.py

_usrp_swig_la_SOURCES =	\
	usrp_swig.cc			

_usrp_swig_la_LIBADD = \
	$(PYTHON_LDFLAGS) \
	libgnuradio-usrp.la

_usrp_swig_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

_usrp_swig_la_CXXFLAGS = @swig_CXXFLAGS@

# Special swig arguments
LOCAL_SWIG_DEFINES_AND_INCLUDES = $(USRP_INCLUDES)

usrp_swig.cc usrp_swig.py: $(ALL_IFILES)
	$(SWIG) $(STD_SWIG_PYTHON_ARGS) -module usrp_swig \
		-o usrp_swig.cc $(srcdir)/usrp.i

noinst_PYTHON =	\
	qa_usrp.py			

swiginclude_HEADERS = \
	$(LOCAL_IFILES)

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done

# ----------------------------------------------------------------
# Misc. build/installation activities

MOSTLYCLEANFILES = $(BUILT_SOURCES) *~ *.pyc
EXTRA_DIST = run_tests.in
TESTS = run_tests
