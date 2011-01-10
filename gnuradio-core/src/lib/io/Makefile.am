#
# Copyright 2001,2003,2004,2006,2007,2008 Free Software Foundation, Inc.
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


AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(WITH_INCLUDES)

noinst_LTLIBRARIES = libio.la


libio_la_SOURCES = 			\
	gr_file_sink.cc			\
	gr_file_sink_base.cc		\
	gr_file_source.cc		\
	gr_file_descriptor_sink.cc	\
	gr_file_descriptor_source.cc	\
	gr_histo_sink_f.cc		\
	gr_message_sink.cc		\
	gr_message_source.cc		\
	gr_oscope_guts.cc		\
	gr_oscope_sink_f.cc		\
	gr_oscope_sink_x.cc		\
	i2c.cc				\
	i2c_bitbang.cc			\
	i2c_bbio.cc			\
	i2c_bbio_pp.cc			\
	microtune_4702.cc		\
	microtune_4937.cc		\
	microtune_4702_eval_board.cc	\
	microtune_4937_eval_board.cc	\
	microtune_xxxx.cc		\
	microtune_xxxx_eval_board.cc	\
	ppio.cc				\
	ppio_ppdev.cc			\
	sdr_1000.cc			\
	gr_udp_sink.cc			\
	gr_udp_source.cc                \
	gr_wavfile_sink.cc              \
	gr_wavfile_source.cc            \
	gri_wavfile.cc			\
	gr_tagged_file_sink.cc

grinclude_HEADERS = 			\
	gr_file_sink.h			\
	gr_file_sink_base.h		\
	gr_file_source.h		\
	gr_file_descriptor_sink.h	\
	gr_file_descriptor_source.h	\
	gr_histo_sink_f.h		\
	gr_message_sink.h		\
	gr_message_source.h		\
	gr_oscope_guts.h		\
	gr_oscope_sink_f.h		\
	gr_oscope_sink_x.h		\
	gr_trigger_mode.h		\
	i2c.h				\
	i2c_bitbang.h			\
	i2c_bbio.h			\
	i2c_bbio_pp.h			\
	microtune_4702.h		\
	microtune_4937.h		\
	microtune_4702_eval_board.h	\
	microtune_4937_eval_board.h	\
	microtune_eval_board_defs.h 	\
	microtune_xxxx.h		\
	microtune_xxxx_eval_board.h	\
	ppio.h				\
	ppio_ppdev.h			\
	sdr_1000.h			\
	gr_udp_sink.h			\
	gr_udp_source.h                 \
	gr_wavfile_source.h	        \
	gr_wavfile_sink.h               \
	gri_wavfile.h			\
	gr_tagged_file_sink.h

swiginclude_HEADERS =			\
	io.i				\
	gr_file_sink.i			\
	gr_file_sink_base.i		\
	gr_file_source.i		\
	gr_file_descriptor_sink.i	\
	gr_file_descriptor_source.i	\
	gr_histo_sink.i			\
	gr_message_sink.i		\
	gr_message_source.i		\
	gr_oscope_sink.i		\
	microtune_xxxx_eval_board.i	\
	microtune_4702_eval_board.i	\
	microtune_4937_eval_board.i	\
	ppio.i				\
	sdr_1000.i			\
	gr_udp_sink.i			\
	gr_udp_source.i                 \
	gr_wavfile_source.i             \
	gr_wavfile_sink.i		\
	gr_tagged_file_sink.i
