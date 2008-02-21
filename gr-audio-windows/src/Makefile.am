#
# Copyright 2004,2005,2008 Free Software Foundation, Inc.
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

# Install this stuff so that it ends up as the gnuradio.audio_windows module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

EXTRA_DIST = run_tests.in
TESTS = run_tests

LOCAL_IFILES = 				\
	$(top_srcdir)/gr-audio-windows/src/audio_windows.i				

NON_LOCAL_IFILES =			\
	$(GNURADIO_I)

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

BUILT_SOURCES = 			\
	audio_windows.cc			\
	audio_windows.py				

ourpython_PYTHON =			\
	audio_windows.py

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

SWIGCPPPYTHONARGS = -python $(PYTHON_CPPFLAGS) $(STD_DEFINES_AND_INCLUDES) \
	 $(WITH_SWIG_INCLUDES)

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
	$(GNURADIO_CORE_LA)		\
	$(WINAUDIO_LIBS)		\
	-lstdc++				

_audio_windows_la_LDFLAGS = -no-undefined -module -avoid-version \
			     $(LIBGNURADIO_CORE_EXTRA_LDFLAGS)

audio_windows.cc audio_windows.py: $(LOCAL_IFILES)
	$(SWIG) $(SWIGCPPPYTHONARGS) -module audio_windows -o audio_windows.cc $(LOCAL_IFILES)


noinst_PYTHON = qa_audio_windows.py

MOSTLYCLEANFILES = \
	$(BUILT_SOURCES) *~ *.pyc

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done
