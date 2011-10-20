#
# Copyright 2008 Free Software Foundation, Inc.
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

LIBS = -lm

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(GRUEL_INCLUDES) \
	-I$(top_srcdir)/gnuradio-core/src/lib/general \
	$(WITH_INCLUDES)

noinst_LTLIBRARIES = libviterbi.la

libviterbi_la_SOURCES = \
    metrics.c		\
    tab.c		\
    viterbi.c

noinst_HEADERS =	\
    viterbi.h

noinst_PROGRAMS = encode decode

encode_SOURCES = encode.cc

encode_LDADD = libviterbi.la

decode_SOURCES = decode.cc

decode_LDADD = libviterbi.la
