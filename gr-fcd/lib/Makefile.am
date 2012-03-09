#
# Copyright 2004,2005,2006,2008,2009,2010,2012 Free Software Foundation, Inc.
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
	$(USB_CPPFLAGS) \
	-I$(top_srcdir)/gr-audio/include

lib_LTLIBRARIES = \
	libgnuradio-fcd.la

libgnuradio_fcd_la_SOURCES = \
	fcd_source_c.cc \
	fcd.c

if FCD_MACOSX
    libgnuradio_fcd_la_SOURCES += hidmac.c
else if FCD_WINDOWS
    libgnuradio_fcd_la_SOURCES += hidwin.c
else
    libgnuradio_fcd_la_SOURCES += hid-libusb.c
endif

noinst_HEADERS = \
	fcd.h \
	fcdhidcmd.h \
	hidapi.h

libgnuradio_fcd_la_LIBADD = \
	$(GNURADIO_CORE_LA) \
	$(top_builddir)/gr-audio/lib/libgnuradio-audio.la \
	$(GNURADIO_AUDIO_LA) \
	$(USB_LIBS)

libgnuradio_fcd_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)

grinclude_HEADERS = \
	fcd_source_c.h
