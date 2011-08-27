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

libgnuradio_audio_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)

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

########################################################################
## OSS Support
########################################################################
if GR_AUDIO_OSS_SUPPORT

AM_CPPFLAGS += \
	-I$(srcdir)/oss

libgnuradio_audio_la_LIBADD += $(OSS_LIBS)

libgnuradio_audio_la_SOURCES += \
	oss/audio_oss_source.cc \
	oss/audio_oss_sink.cc

noinst_HEADERS += \
	oss/audio_oss_source.h \
	oss/audio_oss_sink.h

dist_etc_DATA += oss/gr-audio-oss.conf

endif

########################################################################
## Jack Support
########################################################################
if GR_AUDIO_JACK_SUPPORT

AM_CPPFLAGS += \
	-I$(srcdir)/jack \
	$(JACK_CPPFLAGS)

libgnuradio_audio_la_LIBADD += $(JACK_LIBS)

libgnuradio_audio_la_SOURCES += \
	jack/gri_jack.cc \
	jack/audio_jack_source.cc \
	jack/audio_jack_sink.cc

noinst_HEADERS += \
	jack/gri_jack.h \
	jack/audio_jack_source.h \
	jack/audio_jack_sink.h

dist_etc_DATA += jack/gr-audio-jack.conf

endif

########################################################################
## OSX Support
########################################################################
if GR_AUDIO_OSX_SUPPORT

AM_CPPFLAGS += \
	-I$(srcdir)/osx

libgnuradio_audio_la_LDFLAGS += \
       -framework AudioUnit    \
       -framework CoreAudio    \
       -framework AudioToolbox

libgnuradio_audio_la_SOURCES += \
	osx/audio_osx_source.cc \
	osx/audio_osx_sink.cc

noinst_HEADERS += \
	osx/audio_osx.h \
	osx/audio_osx_source.h \
	osx/audio_osx_sink.h \
	osx/circular_buffer.h

endif

########################################################################
## PortAudio Support
########################################################################
if GR_AUDIO_PORTAUDIO_SUPPORT

AM_CPPFLAGS += \
	-I$(srcdir)/portaudio \
	$(PORTAUDIO_CPPFLAGS)

libgnuradio_audio_la_LIBADD += $(PORTAUDIO_LIBS)

libgnuradio_audio_la_SOURCES += \
	portaudio/gri_portaudio.cc \
	portaudio/audio_portaudio_source.cc \
	portaudio/audio_portaudio_sink.cc

noinst_HEADERS += \
	portaudio/gri_portaudio.h \
	portaudio/audio_portaudio_source.h \
	portaudio/audio_portaudio_sink.h

dist_etc_DATA += portaudio/gr-audio-portaudio.conf

endif

########################################################################
## Windows Support
########################################################################
if GR_AUDIO_WINDOWS_SUPPORT

AM_CPPFLAGS += \
	-I$(srcdir)/windows

libgnuradio_audio_la_LIBADD += $(WINAUDIO_LIBS)

libgnuradio_audio_la_SOURCES += \
	windows/audio_windows_source.cc \
	windows/audio_windows_sink.cc

noinst_HEADERS += \
	windows/audio_windows_source.h \
	windows/audio_windows_sink.h

endif
