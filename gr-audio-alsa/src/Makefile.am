#
# Copyright 2004 Free Software Foundation, Inc.
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

# Install this stuff so that it ends up as the gnuradio.audio_alsa module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

LIBS += $(GNURADIO_CORE_LIBS)

EXTRA_DIST = run_tests.in

TESTS = run_tests

LOCAL_IFILES = 				\
	audio_alsa.i				

NON_LOCAL_IFILES =			\
	$(top_srcdir)/gnuradio-core/src/lib/swig/gnuradio.i

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

BUILT_SOURCES = 			\
	audio_alsa.cc			\
	audio_alsa.py				

ourpython_PYTHON =			\
	audio_alsa.py

INCLUDES = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS)

SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(STD_DEFINES_AND_INCLUDES)

ourlib_LTLIBRARIES = _audio_alsa.la

_audio_alsa_la_SOURCES = 		\
	audio_alsa.cc			\
	audio_alsa_sink.cc		\
	audio_alsa_source.cc		\
	gri_alsa.cc			

grinclude_HEADERS =			\
	audio_alsa_sink.h		\
	audio_alsa_source.h		

noinst_HEADERS = 			\
	gri_alsa.h			


swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)

_audio_alsa_la_LIBADD = 		\
	$(PYTHON_LDFLAGS)		\
	-lstdc++				

_audio_alsa_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

audio_alsa.cc audio_alsa.py: audio_alsa.i $(NON_LOCAL_IFILES)
	$(SWIG) $(SWIGPYTHONARGS) -module audio_alsa -o audio_alsa.cc $<


noinst_PYTHON = 			\
	qa_alsa.py			

MOSTLYCLEANFILES = \
	$(BUILT_SOURCES) *~ *.pyc
