#
# Copyright 2011,2012 Free Software Foundation, Inc.
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
	-I$(top_srcdir)/gr-fcd/include/fcd \
	-I$(top_srcdir)/gr-audio/include \
	$(STD_DEFINES_AND_INCLUDES) \
	$(WITH_INCLUDES)

GR_FCD_LA=$(top_builddir)/gr-fcd/lib/libgnuradio-fcd.la
GNURADIO_AUDIO_LA=$(top_builddir)/gr-audio/lib/libgnuradio-audio.la

ourdatadir = $(exampledir)/fcd

ourdata_PROGRAMS = \
	fcd_nfm_rx

fcd_nfm_rx_SOURCES = \
	fcd_nfm_rx.cc

fcd_nfm_rx_LDADD = \
	$(GR_FCD_LA) \
	$(GNURADIO_CORE_LA) \
	$(GNURADIO_AUDIO_LA) \
	$(BOOST_PROGRAM_OPTIONS_LIB)

dist_ourdata_SCRIPTS = \
    fcd_fft_wx.grc \
    fcd_apt_rx.grc \
    fcd_nfm_rx.grc

