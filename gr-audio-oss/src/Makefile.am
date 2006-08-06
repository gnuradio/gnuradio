#
# Copyright 2004,2005,2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

include $(top_srcdir)/Makefile.common

LIBS += $(GNURADIO_CORE_LIBS)

# Install this stuff so that it ends up as the gnuradio.audio_oss module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

EXTRA_DIST = run_tests.in
TESTS = run_tests

LOCAL_IFILES = 				\
	audio_oss.i				

NON_LOCAL_IFILES =			\
	$(top_srcdir)/gnuradio-core/src/lib/swig/gnuradio.i

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

BUILT_SOURCES = 			\
	audio_oss.cc			\
	audio_oss.py			

ourpython_PYTHON =			\
	audio_oss.py

INCLUDES = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS)

SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(STD_DEFINES_AND_INCLUDES)

ourlib_LTLIBRARIES = _audio_oss.la

_audio_oss_la_SOURCES = 		\
	audio_oss.cc			\
	audio_oss_sink.cc		\
	audio_oss_source.cc			


grinclude_HEADERS =			\
	audio_oss_sink.h		\
	audio_oss_source.h			

swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)

_audio_oss_la_LIBADD = 			\
	$(PYTHON_LDFLAGS)		\
	$(OSS_LIBS)			\
	-lstdc++				

_audio_oss_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

audio_oss.cc audio_oss.py: audio_oss.i $(NON_LOCAL_IFILES)
	$(SWIG) $(SWIGPYTHONARGS) -module audio_oss -o audio_oss.cc $<

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done

noinst_PYTHON = qa_oss.py

MOSTLYCLEANFILES = \
	$(BUILT_SOURCES) *~ *.pyc

