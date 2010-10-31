#
# Copyright 2004,2008,2009 Free Software Foundation, Inc.
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

EXTRA_DIST = run_tests.in

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) \
	$(PORTAUDIO_CFLAGS) $(WITH_INCLUDES)

# C/C++ headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			\
	audio_portaudio_sink.h		\
	audio_portaudio_source.h		

noinst_HEADERS = 			\
	gri_portaudio.h

noinst_PYTHON = 			\
	qa_portaudio.py

lib_LTLIBRARIES = libgnuradio-audio-portaudio.la

libgnuradio_audio_portaudio_la_SOURCES = \
	audio_portaudio_sink.cc		\
	audio_portaudio_source.cc	\
	gri_portaudio.cc

libgnuradio_audio_portaudio_la_LIBADD =	\
	$(GNURADIO_CORE_LA)		\
	$(PORTAUDIO_LIBS)

libgnuradio_audio_portaudio_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)


if PYTHON
###################################
# SWIG Python interface and library
TESTS = run_tests
DISTCLEANFILES = run_tests

TOP_SWIG_IFILES = 			\
	audio_portaudio.i

# Install so that they end up available as:
#   import gnuradio.audio_portaudio
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio
audio_portaudio_pythondir_category =	\
	gnuradio

# additional libraries for linking with the SWIG-generated library
audio_portaudio_la_swig_libadd =	\
	libgnuradio-audio-portaudio.la

include $(top_srcdir)/Makefile.swig

# add some of the variables generated inside the Makefile.swig.gen
BUILT_SOURCES = $(swig_built_sources)

# Do not distribute the output of SWIG
no_dist_files = $(swig_built_sources)
endif
