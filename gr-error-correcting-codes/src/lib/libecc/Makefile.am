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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

include $(top_srcdir)/Makefile.common

SUBDIRS = mld . tests

INCLUDES = $(STD_DEFINES_AND_INCLUDES) -I$(top_srcdir)/gr-error-correcting-codes/src/lib

noinst_LTLIBRARIES = libecc.la

libecc_la_SOURCES = 				\
	code_convolutional_trellis.cc		\
	code_metrics.cc	code_io.cc		\
	encoder.cc				\
	encoder_convolutional.cc 		\
	encoder_turbo.cc			\
	decoder.cc 				\
	decoder_viterbi.cc			\
	decoder_viterbi_full_block.cc

noinst_HEADERS =				\
	code_types.h code_metrics.h code_io.h	\
	code_convolutional_trellis.h		\
	encoder.h encoder_turbo.h		\
	encoder_convolutional.h 		\
	decoder.h decoder_viterbi.h		\
	decoder_viterbi_full_block.h

# link the library against the c++ standard library
libecc_la_LIBADD = 		\
	mld/libmld.la		\
	$(PYTHON_LDFLAGS)	\
	-lstdc++

MOSTLYCLEANFILES = *.loT *~

CONFIG_CLEAN_FILES = *.in
