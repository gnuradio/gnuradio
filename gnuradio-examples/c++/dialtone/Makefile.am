#
# Copyright 2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

# For compiling within the GNU Radio build tree
INCLUDES=$(STD_DEFINES_AND_INCLUDES) \
         -I$(top_srcdir)/gr-audio-alsa/src 

GR_AUDIO_ALSA_LIBS=$(top_builddir)/gr-audio-alsa/src/libgr_audio_alsa.la

# For compiling outside the tree, these are the usual
# INCLUDES = -I/usr/local/include -I/usr/local/include/gnuradio
# GNURADIO_CORE_LIBS = -lgnuradio-core
# GR_AUDIO_ALSA_LIBS = -lgr_audio_alsa

noinst_PROGRAMS = dialtone
    
noinst_HEADERS = \
    dialtone.h
        
dialtone_SOURCES = \
    dialtone.cc	   \
    main.cc

dialtone_LDADD = \
    $(GNURADIO_CORE_LIBS) \
    $(GR_AUDIO_ALSA_LIBS)

MOSTLYCLEANFILES = *~
