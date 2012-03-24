#
# Copyright 2010-2012 Free Software Foundation, Inc.
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
	$(UHD_CPPFLAGS) \
	-I$(srcdir) \
	-Dgnuradio_uhd_EXPORTS

lib_LTLIBRARIES = libgnuradio-uhd.la

libgnuradio_uhd_la_SOURCES = \
	gr_uhd_usrp_source.cc \
	gr_uhd_usrp_sink.cc \
	gr_uhd_amsg_source.cc

libgnuradio_uhd_la_LIBADD = \
	$(GNURADIO_CORE_LA) \
	$(UHD_LIBS)

libgnuradio_uhd_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)

noinst_HEADERS = gr_uhd_common.h
