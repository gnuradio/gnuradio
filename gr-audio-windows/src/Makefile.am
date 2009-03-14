#
# Copyright 2004,2005,2008,2009 Free Software Foundation, Inc.
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

TESTS = run_tests

DISTCLEANFILES = run_tests

noinst_PYTHON = qa_audio_windows.py

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

# C/C++ headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			\
	audio_windows_sink.h		\
	audio_windows_source.h

###################################
# SWIG Python interface and library

TOP_SWIG_IFILES = 			\
	audio_windows.i

# Install so that they end up available as:
#   import gnuradio.audio_windows
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio
audio_windows_pythondir_category =	\
	gnuradio

# additional sources for the SWIG-generated library
audio_windows_la_swig_sources = 	\
	audio_windows_sink.cc		\
	audio_windows_source.cc		

# additional libraries for linking with the SWIG-generated library
audio_windows_la_swig_libadd =		\
	$(GNURADIO_CORE_LA)		\
	$(WINAUDIO_LIBS)

# additional LD flags for linking the SWIG-generated library
audio_windows_la_swig_ldflags =		\
	$(LIBGNURADIO_CORE_EXTRA_LDFLAGS)

include $(top_srcdir)/Makefile.swig

# add some of the variables generated inside the Makefile.swig.gen
BUILT_SOURCES = $(swig_built_sources)

# Do not distribute the output of SWIG
no_dist_files = $(swig_built_sources)
