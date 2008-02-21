#
# Copyright 2006,2008 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio.
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

# Install this stuff so that it ends up as the gnuradio.audio_osx module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

EXTRA_DIST = run_tests.in
TESTS = run_tests

LOCAL_IFILES = 				\
	$(top_srcdir)/gr-audio-osx/src/audio_osx.i				

NON_LOCAL_IFILES =			\
	$(GNURADIO_I)

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

BUILT_SOURCES = 			\
	audio_osx.cc			\
	audio_osx.py				

ourpython_PYTHON =			\
	audio_osx.py

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

SWIGCPPPYTHONARGS = -python $(PYTHON_CPPFLAGS) $(STD_DEFINES_AND_INCLUDES) \
	 $(WITH_SWIG_INCLUDES) $(WITH_INCLUDES)

ourlib_LTLIBRARIES = _audio_osx.la

_audio_osx_la_SOURCES = 		\
	audio_osx.cc			\
	audio_osx_sink.cc		\
	audio_osx_source.cc			

noinst_HEADERS =			\
	audio_osx.h			\
	circular_buffer.h		\
	mld_threads.h

grinclude_HEADERS =			\
	audio_osx_sink.h		\
	audio_osx_source.h			

swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)

_audio_osx_la_LIBADD = 			\
	$(PYTHON_LDFLAGS)		\
	$(GNURADIO_CORE_LA)		\
	-lstdc++				

_audio_osx_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version -framework AudioUnit -framework CoreAudio -framework AudioToolbox

audio_osx.cc audio_osx.py: $(ALL_IFILES)
	$(SWIG) $(SWIGCPPPYTHONARGS) -module audio_osx -o audio_osx.cc $(LOCAL_IFILES)

noinst_PYTHON = qa_osx.py test_audio_loop.py

MOSTLYCLEANFILES = $(BUILT_SOURCES) *~ *.pyc run_tests *.loT

CONFIG_CLEAN_FILES = run_tests *.loT

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done
