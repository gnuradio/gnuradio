#
# Copyright 2001,2002,2004,2006,2007,2008 Free Software Foundation, Inc.
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

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(CPPUNIT_INCLUDES) $(WITH_INCLUDES)

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
	gr_argmax_XX.cc.t		\
	gr_argmax_XX.h.t		\
	gr_argmax_XX.i.t		\
	gr_chunks_to_symbols_XX.cc.t	\
	gr_chunks_to_symbols_XX.h.t	\
	gr_chunks_to_symbols_XX.i.t	\
	gr_divide_XX.cc.t		\
	gr_divide_XX.h.t		\
	gr_divide_XX.i.t		\
	gr_integrate_XX.cc.t		\
	gr_integrate_XX.h.t		\
	gr_integrate_XX.i.t		\
	gr_max_XX.cc.t			\
	gr_max_XX.h.t			\
	gr_max_XX.i.t			\
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
	gr_peak_detector_XX.cc.t	\
	gr_peak_detector_XX.h.t		\
	gr_peak_detector_XX.i.t		\
	gr_sample_and_hold_XX.cc.t	\
	gr_sample_and_hold_XX.h.t	\
	gr_sample_and_hold_XX.i.t	\
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
	gr_vector_sink_X.i.t		\
	gr_xor_XX.cc.t			\
	gr_xor_XX.h.t			\
	gr_xor_XX.i.t     		\
	gr_and_XX.cc.t			\
	gr_and_XX.h.t			\
	gr_and_XX.i.t     		\
	gr_or_XX.cc.t			\
	gr_or_XX.h.t			\
	gr_or_XX.i.t      		\
	gr_not_XX.cc.t			\
	gr_not_XX.h.t			\
	gr_not_XX.i.t			\
	gr_moving_average_XX.cc.t	\
	gr_moving_average_XX.h.t	\
	gr_moving_average_XX.i.t

include Makefile.gen

# Ensure parallel make does the right thing.
# http://sources.redhat.com/automake/automake.html#Multiple-Outputs

STAMPS = generate-stamp

generate-stamp: $(CODE_GENERATOR)
	@rm -f generate-tmp
	@touch generate-tmp
	PYTHONPATH=$(top_srcdir)/gnuradio-core/src/python srcdir=$(srcdir) $(PYTHON) $(srcdir)/generate_all.py
	@mv -f generate-tmp $@


$(GENERATED_H) $(GENERATED_I) $(GENERATED_CC) gengen_generated.i : generate-stamp
## Recover from the removal of $@
	@if test -f $@; then :; else \
               trap 'rm -rf generate-lock generate-stamp' 1 2 13 15; \
               if mkdir generate-lock 2>/dev/null; then \
## This code is being executed by the first process.
                 rm -f generate-stamp; \
                 $(MAKE) $(AM_MAKEFLAGS) generate-stamp; \
                 rmdir generate-lock; \
               else \
## This code is being executed by the follower processes.
## Wait until the first process is done.
                 while test -d generate-lock; do sleep 1; done; \
## Succeed if and only if the first process succeeded.
                 test -f generate-stamp; exit $$?; \
               fi; \
             fi


BUILT_SOURCES = $(GENERATED_H) $(GENERATED_I) $(GENERATED_CC)

# ----------------------------------------------------------------

EXTRA_DIST = 				\
	$(CODE_GENERATOR)		\
	$(STAMPS)

libgengen_la_SOURCES = 			\
	$(GENERATED_CC)			

#libgengen_qa_la_SOURCES = 		

grinclude_HEADERS = 			\
	$(GENERATED_H)			\
	gr_endianness.h			\
	gr_noise_type.h			\
	gr_sig_source_waveform.h	

noinst_HEADERS = 			

swiginclude_HEADERS =			\
	$(GENERATED_I)			\
	gr_endianness.i			\
	gengen.i			\
	gengen_generated.i		

CLEANFILES = $(BUILT_SOURCES) $(STAMPS) *.pyc
