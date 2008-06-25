#
# Copyright 2004,2008 Free Software Foundation, Inc.
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

# Install this stuff so that it ends up as the gnuradio.audio_portaudio module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

EXTRA_DIST = run_tests.in

TESTS = run_tests

LOCAL_IFILES = 				\
	$(top_srcdir)/gr-audio-portaudio/src/audio_portaudio.i

NON_LOCAL_IFILES =			\
	$(GNURADIO_I)

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

BUILT_SOURCES = 			\
	audio_portaudio.cc		\
	audio_portaudio.py				

ourpython_PYTHON =			\
	audio_portaudio.py

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) \
	$(PORTAUDIO_CFLAGS) $(WITH_INCLUDES)

SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(STD_DEFINES_AND_INCLUDES) \
	-I/usr/include $(WITH_SWIG_INCLUDES) $(WITH_INCLUDES)

ourlib_LTLIBRARIES = _audio_portaudio.la

_audio_portaudio_la_SOURCES = 		\
	audio_portaudio.cc		\
	audio_portaudio_sink.cc		\
	audio_portaudio_source.cc	\
	gri_portaudio.cc			

grinclude_HEADERS =			\
	audio_portaudio_sink.h		\
	audio_portaudio_source.h		

noinst_HEADERS = 			\
	gri_portaudio.h

swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)

_audio_portaudio_la_LIBADD = 		\
	$(PYTHON_LDFLAGS)		\
	$(GNURADIO_CORE_LA)		\
	$(PORTAUDIO_LIBS)		\
	-lstdc++

_audio_portaudio_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

audio_portaudio.cc audio_portaudio.py: $(ALL_IFILES) audio_portaudio_sink.h audio_portaudio_source.h
	$(SWIG) $(SWIGPYTHONARGS) -module audio_portaudio -o audio_portaudio.cc $(LOCAL_IFILES)

noinst_PYTHON = 			\
	qa_portaudio.py

MOSTLYCLEANFILES = \
	$(BUILT_SOURCES) *~ *.pyc

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done
