#
# Copyright 2004,2005,2006,2008,2009,2010 Free Software Foundation, Inc.
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
	audio_oss_sink.h	\
	audio_oss_source.h

noinst_PYTHON = qa_oss.py

lib_LTLIBRARIES = libgnuradio-audio-oss.la

libgnuradio_audio_oss_la_SOURCES = 	\
	audio_oss_sink.cc		\
	audio_oss_source.cc

libgnuradio_audio_oss_la_LIBADD =	\
	$(GNURADIO_CORE_LA)		\
	$(OSS_LIBS)

libgnuradio_audio_oss_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)

###################################
# SWIG interface and library
TOP_SWIG_IFILES =		\
	audio_oss.i

# Install so that they end up available as:
#   import gnuradio.audio_oss
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio
audio_oss_pythondir_category =	\
	gnuradio

# additional libraries for linking with the SWIG-generated library
audio_oss_la_swig_libadd =	\
	libgnuradio-audio-oss.la


if PYTHON
TESTS += run_tests
endif
