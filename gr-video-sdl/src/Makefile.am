#
# Copyright 2004,2005,2006,2008,2009 Free Software Foundation, Inc.
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

noinst_PYTHON =				\
	qa_video_sdl.py

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(SDL_CFLAGS) \
	$(WITH_INCLUDES)

grinclude_HEADERS =			\
	video_sdl_sink_uc.h		\
	video_sdl_sink_s.h

#################################
# SWIG interface and library

TOP_SWIG_IFILES =			\
	video_sdl.i

# Install so that they end up available as:
#   import gnuradio.video_sdl
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio
video_sdl_pythondir_category =		\
	gnuradio

# additional sources for the SWIG-generated library
video_sdl_la_swig_sources =		\
	video_sdl_sink_uc.cc		\
	video_sdl_sink_s.cc

# additional libraries for linking with the SWIG-generated library
video_sdl_la_swig_libadd =		\
	$(SDL_LIBS)			\
	$(GNURADIO_CORE_LA)

include $(top_srcdir)/Makefile.swig

# add some of the variables generated inside the Makefile.swig.gen
BUILT_SOURCES = $(swig_built_sources)

# Do not distribute the output of SWIG
no_dist_files = $(swig_built_sources)
