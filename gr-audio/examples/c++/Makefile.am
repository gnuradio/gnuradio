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

# For compiling with the GNU Radio build tree, use include dir below.
# For compiling outside the tree, use 'pkg-config --cflags gnuradio-audio'
AM_CPPFLAGS = \
	-I$(top_srcdir)/gr-audio/include \
	$(STD_DEFINES_AND_INCLUDES) \
	$(WITH_INCLUDES)

# For compiling within the GNU Radio build tree, link against GR_AUDIO_LA
# For compiling outside the tree, use 'pkg-config --libs gnuradio-audio'
GR_AUDIO_LA=$(top_builddir)/gr-audio/lib/libgnuradio-audio.la

noinst_PROGRAMS = dial_tone

dial_tone_SOURCES = dial_tone.cc
dial_tone_LDADD = \
	$(GNURADIO_CORE_LA) $(GR_AUDIO_LA) \
	$(BOOST_FILESYSTEM_LIB)
