#
# Copyright 2011 Free Software Foundation, Inc.
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
	-I$(abs_top_srcdir)/gr-audio/include \
	-Dgnuradio_audio_EXPORTS

lib_LTLIBRARIES = libgnuradio-audio.la

libgnuradio_audio_la_SOURCES = \
	gr_audio_registry.cc

libgnuradio_audio_la_LIBADD = \
	$(GNURADIO_CORE_LA)

libgnuradio_audio_la_LDFLAGS = $(LTVERSIONFLAGS)

noinst_HEADERS = gr_audio_registry.h

etcdir = $(gr_prefsdir)
dist_etc_DATA = gr-audio.conf

########################################################################
## ALSA Support
########################################################################
if GR_AUDIO_ALSA_SUPPORT

AM_CPPFLAGS += \
	-I$(srcdir)/alsa \
	$(ALSA_CPPFLAGS)

libgnuradio_audio_la_LIBADD += $(ALSA_LIBS)

libgnuradio_audio_la_SOURCES += \
	alsa/gri_alsa.cc \
	alsa/audio_alsa_source.cc \
	alsa/audio_alsa_sink.cc

noinst_HEADERS += \
	alsa/gri_alsa.h \
	alsa/audio_alsa_source.h \
	alsa/audio_alsa_sink.h

dist_etc_DATA += alsa/gr-audio-alsa.conf

endif
