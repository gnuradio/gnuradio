#
# Copyright 2004,2005 Free Software Foundation, Inc.
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

# Install this stuff so that it ends up as the gnuradio.audio_windows module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

EXTRA_DIST = run_tests.in
TESTS = run_tests

LOCAL_IFILES = 				\
	audio_windows.i				

NON_LOCAL_IFILES =			\
	$(top_srcdir)/gnuradio-core/src/lib/swig/gnuradio.i

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

BUILT_SOURCES = 			\
	audio_windows.cc			\
	audio_windows.py				

ourpython_PYTHON =			\
	audio_windows.py

INCLUDES = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS)

SWIGCPPPYTHONARGS = -python $(PYTHON_CPPFLAGS) \
		    -I$(swigincludedir) -I$(grincludedir)

ourlib_LTLIBRARIES = _audio_windows.la

_audio_windows_la_SOURCES = 		\
	audio_windows.cc			\
	audio_windows_sink.cc		\
	audio_windows_source.cc			


grinclude_HEADERS =			\
	audio_windows_sink.h		\
	audio_windows_source.h			

swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)

_audio_windows_la_LIBADD = 		\
	$(PYTHON_LDFLAGS)		\
	$(GNURADIO_CORE_LIBS)		\
	-lwinmm				\
	-lstdc++				

_audio_windows_la_LDFLAGS = -no-undefined -module -avoid-version \
			     $(LIBGNURADIO_CORE_EXTRA_LDFLAGS)

audio_windows.cc audio_windows.py: audio_windows.i
	$(SWIG) $(SWIGCPPPYTHONARGS) -module audio_windows -o audio_windows.cc $<


noinst_PYTHON = qa_audio_windows.py

MOSTLYCLEANFILES = \
	$(BUILT_SOURCES) *~ *.pyc
