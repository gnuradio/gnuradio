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
include $(top_srcdir)/Makefile.swig

TESTS =
EXTRA_DIST += run_tests.in
DISTCLEANFILES += run_tests

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(SDL_CFLAGS) \
	$(WITH_INCLUDES)

noinst_PYTHON =				\
	qa_video_sdl.py

grinclude_HEADERS =			\
	video_sdl_api.h			\
	video_sdl_sink_uc.h		\
	video_sdl_sink_s.h

lib_LTLIBRARIES = libgnuradio-video-sdl.la

libgnuradio_video_sdl_la_SOURCES = 	\
	video_sdl_sink_uc.cc		\
	video_sdl_sink_s.cc

libgnuradio_video_sdl_la_LIBADD =	\
	$(SDL_LIBS)			\
	$(GNURADIO_CORE_LA)

libgnuradio_video_sdl_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)


#################################
# SWIG interfaces and libraries

TOP_SWIG_DOC_IFILES = \
	video_sdl_swig_doc.i

TOP_SWIG_IFILES =			\
	$(TOP_SWIG_DOC_IFILES) 	\
	video_sdl.i

BUILT_SOURCES += \
	$(TOP_SWIG_DOC_IFILES)

EXTRA_DIST += \
	$(TOP_SWIG_DOC_IFILES)

$(TOP_SWIG_DOC_IFILES):
	`echo "" > $@`

# Install so that they end up available as:
#   import gnuradio.video_sdl
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio
video_sdl_pythondir_category =		\
	gnuradio

# additional libraries for linking with the SWIG-generated library
video_sdl_la_swig_libadd =		\
	libgnuradio-video-sdl.la

video_sdl_swiginclude_headers =		\
	$(TOP_SWIG_DOC_IFILES)

if PYTHON
TESTS += run_tests
endif
