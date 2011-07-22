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

SUBDIRS = codec2 g7xx gsm .

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(WITH_INCLUDES) \
	      -I$(top_srcdir)/gr-vocoder/include

lib_LTLIBRARIES = libgnuradio-vocoder.la

libgnuradio_vocoder_la_SOURCES = \
	vocoder_alaw_decode_bs.cc \
	vocoder_alaw_encode_sb.cc \
	vocoder_codec2_decode_ps.cc \
	vocoder_codec2_encode_sp.cc \
	vocoder_cvsd_decode_bs.cc \
	vocoder_cvsd_encode_sb.cc \
	vocoder_g721_decode_bs.cc \
	vocoder_g721_encode_sb.cc \
	vocoder_g723_24_decode_bs.cc \
	vocoder_g723_24_encode_sb.cc \
	vocoder_g723_40_decode_bs.cc \
	vocoder_g723_40_encode_sb.cc \
	vocoder_gsm_fr_decode_ps.cc \
	vocoder_gsm_fr_encode_sp.cc \
	vocoder_ulaw_decode_bs.cc \
	vocoder_ulaw_encode_sb.cc

libgnuradio_vocoder_la_LIBADD =	\
	$(GNURADIO_CORE_LA) \
	codec2/libcodec2.la \
	g7xx/libg7xx.la \
	gsm/libgsm.la

libgnuradio_vocoder_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)
