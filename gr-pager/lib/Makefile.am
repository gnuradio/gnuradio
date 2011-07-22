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

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS = \
	pager_api.h \
	pager_slicer_fb.h \
	pager_flex_sync.h \
	pager_flex_deinterleave.h \
	pager_flex_parse.h \
	pager_flex_frame.h \
	pageri_bch3221.h \
	pageri_flex_modes.h \
	pageri_util.h

lib_LTLIBRARIES = libgnuradio-pager.la

libgnuradio_pager_la_SOURCES = \
	pager_flex_frame.cc \
	pager_slicer_fb.cc \
	pager_flex_sync.cc \
	pager_flex_deinterleave.cc \
	pager_flex_parse.cc \
	pageri_bch3221.cc \
	pageri_flex_modes.cc \
	pageri_util.cc

libgnuradio_pager_la_LIBADD =	\
	$(GNURADIO_CORE_LA)

libgnuradio_pager_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)
