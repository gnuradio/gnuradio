#
# Copyright 2001,2002,2004,2006 Free Software Foundation, Inc.
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

INCLUDES = $(STD_DEFINES_AND_INCLUDES) $(CPPUNIT_INCLUDES)

#noinst_LTLIBRARIES = libgengen.la libgengen-qa.la
noinst_LTLIBRARIES = libgengen.la

# ----------------------------------------------------------------
# these scripts generate code

CODE_GENERATOR = 			\
	generate_all.py			\
	generate_common.py		\
	gr_add_XX.cc.t			\
	gr_add_XX.h.t			\
	gr_add_XX.i.t			\
	gr_add_const_XX.cc.t		\
	gr_add_const_XX.h.t		\
	gr_add_const_XX.i.t		\
	gr_add_vXX.cc.t			\
	gr_add_vXX.h.t			\
	gr_add_vXX.i.t			\
	gr_add_const_vXX.cc.t		\
	gr_add_const_vXX.h.t		\
	gr_add_const_vXX.i.t		\
	gr_chunks_to_symbols_XX.cc.t	\
	gr_chunks_to_symbols_XX.h.t	\
	gr_chunks_to_symbols_XX.i.t	\
	gr_divide_XX.cc.t		\
	gr_divide_XX.h.t		\
	gr_divide_XX.i.t		\
	gr_multiply_XX.cc.t		\
	gr_multiply_XX.h.t		\
	gr_multiply_XX.i.t		\
	gr_multiply_const_XX.cc.t	\
	gr_multiply_const_XX.h.t	\
	gr_multiply_const_XX.i.t	\
	gr_multiply_vXX.cc.t		\
	gr_multiply_vXX.h.t		\
	gr_multiply_vXX.i.t		\
	gr_multiply_const_vXX.cc.t	\
	gr_multiply_const_vXX.h.t	\
	gr_multiply_const_vXX.i.t	\
	gr_mute_XX.cc.t			\
	gr_mute_XX.h.t			\
	gr_mute_XX.i.t			\
	gr_noise_source_X.cc.t		\
	gr_noise_source_X.h.t		\
	gr_noise_source_X.i.t		\
	gr_packed_to_unpacked_XX.cc.t	\
	gr_packed_to_unpacked_XX.h.t	\
	gr_packed_to_unpacked_XX.i.t	\
	gr_sig_source_X.cc.t		\
	gr_sig_source_X.h.t		\
	gr_sig_source_X.i.t		\
	gr_sub_XX.cc.t			\
	gr_sub_XX.h.t			\
	gr_sub_XX.i.t			\
	gr_unpacked_to_packed_XX.cc.t	\
	gr_unpacked_to_packed_XX.h.t	\
	gr_unpacked_to_packed_XX.i.t	\
	gr_vector_source_X.cc.t		\
	gr_vector_source_X.h.t		\
	gr_vector_source_X.i.t		\
	gr_vector_sink_X.cc.t		\
	gr_vector_sink_X.h.t		\
	gr_vector_sink_X.i.t		

# include $(srcdir)/Makefile.gen
include Makefile.gen


$(GENERATED_H) $(GENERATED_I) $(GENERATED_CC): $(CODE_GENERATOR)
	PYTHONPATH=$(top_srcdir)/gnuradio-core/src/python srcdir=$(srcdir) $(srcdir)/generate_all.py


BUILT_SOURCES = $(GENERATED_H) $(GENERATED_I) $(GENERATED_CC)

# ----------------------------------------------------------------

EXTRA_DIST = 				\
	$(CODE_GENERATOR)		

libgengen_la_SOURCES = 			\
	$(GENERATED_CC)			

#libgengen_qa_la_SOURCES = 		

grinclude_HEADERS = 			\
	$(GENERATED_H)			\
	gr_endianness.h			\
	gr_sig_source_waveform.h	

noinst_HEADERS = 			

swiginclude_HEADERS =			\
	$(GENERATED_I)			\
	gr_endianness.i			\
	gengen.i			\
	gengen_generated.i		

CLEANFILES = $(BUILT_SOURCES) *.pyc
