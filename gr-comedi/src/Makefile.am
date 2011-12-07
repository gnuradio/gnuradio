#
# Copyright 2005,2008,2009,2010 Free Software Foundation, Inc.
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
EXTRA_DIST += run_tests.in
DISTCLEANFILES += run_tests


AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

# C/C++ headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =		\
	comedi_sink_s.h		\
	comedi_source_s.h

noinst_HEADERS =		\
	gri_comedi.h

noinst_PYTHON =			\
	qa_comedi.py

lib_LTLIBRARIES = libgnuradio-comedi.la

libgnuradio_comedi_la_SOURCES = \
	comedi_sink_s.cc	\
	comedi_source_s.cc	\
	gri_comedi.cc

libgnuradio_comedi_la_LIBADD =	\
	$(GNURADIO_CORE_LA)	\
	$(COMEDI_LIBS)

libgnuradio_comedi_la_LDFLAGS =	$(NO_UNDEFINED) $(LTVERSIONFLAGS)

###################################
# SWIG interfaces and libraries

TOP_SWIG_DOC_IFILES = \
	comedi_swig_doc.i

TOP_SWIG_IFILES =		\
	$(TOP_SWIG_DOC_IFILES) 	\
	comedi.i

BUILT_SOURCES += \
	$(TOP_SWIG_DOC_IFILES)

EXTRA_DIST += \
	$(TOP_SWIG_DOC_IFILES)

$(TOP_SWIG_DOC_IFILES):
	`echo "" > $@`

# Install so that they end up available as:
#   import gnuradio.comedi
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio
comedi_pythondir_category =	\
	gnuradio

# additional libraries for linking with the SWIG-generated library
comedi_la_swig_libadd =		\
	libgnuradio-comedi.la

# additional SWIG files to be installed
comedi_swig_swiginclude_headers = \
	$(TOP_SWIG_DOC_IFILES)

if PYTHON
TESTS += run_tests
endif
