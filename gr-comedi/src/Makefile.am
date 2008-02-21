#
# Copyright 2005,2008 Free Software Foundation, Inc.
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

# Install this stuff so that it ends up as the gnuradio.comedi module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

EXTRA_DIST = run_tests.in

TESTS = run_tests

LOCAL_IFILES = 				\
	$(top_srcdir)/gr-comedi/src/comedi.i				

NON_LOCAL_IFILES =			\
	$(GNURADIO_I)

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

BUILT_SOURCES = 			\
	comedi.cc			\
	comedi.py				

ourpython_PYTHON =			\
	comedi.py

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

SWIGCPPPYTHONARGS = -c++ -python $(PYTHON_CPPFLAGS) \
	$(STD_DEFINES_AND_INCLUDES) $(WITH_SWIG_INCLUDES) $(WITH_INCLUDES)
#	-I$(swigincludedir) $(S-I$(grincludedir) -I$(comediincludedir)

ourlib_LTLIBRARIES = _comedi.la

_comedi_la_SOURCES = 		\
	comedi.cc		\
	comedi_sink_s.cc	\
	comedi_source_s.cc	\
	gri_comedi.cc			

grinclude_HEADERS =		\
	comedi_sink_s.h		\
	comedi_source_s.h		

noinst_HEADERS = 		\
	gri_comedi.h			


swiginclude_HEADERS = 		\
	$(LOCAL_IFILES)

_comedi_la_LIBADD = 		\
	$(PYTHON_LDFLAGS)	\
	$(GNURADIO_CORE_LA)	\
	$(COMEDI_LIBS)		\
	-lstdc++				

_comedi_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

comedi.cc comedi.py: $(LOCAL_IFILES)
	$(SWIG) $(SWIGCPPPYTHONARGS) -module comedi -o comedi.cc $(LOCAL_IFILES)


noinst_PYTHON = 		\
	qa_comedi.py			

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done

MOSTLYCLEANFILES = \
	$(BUILT_SOURCES) *~ *.pyc

