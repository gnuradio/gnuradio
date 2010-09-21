#
# Copyright 2010 Free Software Foundation, Inc.
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

AM_CPPFLAGS = \
	$(STD_DEFINES_AND_INCLUDES) \
	$(WITH_INCLUDES) \
	$(UHD_CFLAGS)

lib_LTLIBRARIES = libgnuradio-uhd.la

libgnuradio_uhd_la_SOURCES = \
	utils.cc \
	uhd_mimo_source.cc \
	uhd_mimo_sink.cc \
	uhd_simple_source.cc \
	uhd_simple_sink.cc \
	uhd_single_usrp_source.cc \
	uhd_single_usrp_sink.cc

libgnuradio_uhd_la_LIBADD = \
	$(GNURADIO_CORE_LA) \
	$(UHD_LIBS)

libgnuradio_uhd_la_LDFLAGS = $(LTVERSIONFLAGS)

grinclude_HEADERS = \
	uhd_mimo_source.h \
	uhd_mimo_sink.h \
	uhd_simple_source.h \
	uhd_simple_sink.h \
	uhd_single_usrp_source.h \
	uhd_single_usrp_sink.h

noinst_HEADERS = \
	utils.h
