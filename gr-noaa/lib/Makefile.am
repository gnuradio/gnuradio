#
# Copyright 2009 Free Software Foundation, Inc.
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
	$(WITH_INCLUDES)

lib_LTLIBRARIES = \
	libgnuradio-noaa.la

libgnuradio_noaa_la_SOURCES = \
	noaa_hrpt_decoder.cc \
	noaa_hrpt_deframer.cc \
	noaa_hrpt_pll_cf.cc

noinst_HEADERS = \
	noaa_hrpt.h

libgnuradio_noaa_la_LIBADD = \
	$(GNURADIO_CORE_LA)

libgnuradio_noaa_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)

grinclude_HEADERS = \
	noaa_api.h \
	noaa_hrpt_decoder.h \
	noaa_hrpt_deframer.h \
	noaa_hrpt_pll_cf.h
