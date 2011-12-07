#
# Copyright 2001,2004,2005,2006,2008,2009,2010 Free Software Foundation, Inc.
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
include $(top_srcdir)/Makefile.swig

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) \
	$(CPPUNIT_INCLUDES) $(WITH_INCLUDES)

EXTRA_DIST +=					\
	atsci_viterbi_gen.cc			\
	gen_encoder.py				\
	qa_atsci_trellis_encoder_t1_input.dat	\
	qa_atsci_trellis_encoder_t1_output.dat	\
	qa_atsci_viterbi_decoder_t1_input.dat	\
	qa_atsci_viterbi_decoder_t1_output.dat


TESTS =	test_atsci

lib_LTLIBRARIES = libgnuradio-atsc.la

# FIXME
#	atsci_exp2_lp.cc			\
#	atsci_root_raised_cosine.cc		\
#	atsci_root_raised_cosine_bandpass.cc	\
#	atsci_vsbtx_lp.cc			\
#

libgnuradio_atsc_la_SOURCES = 			\
	atsc_derandomizer.cc			\
	atsc_randomizer.cc			\
	atsc_rs_decoder.cc			\
	atsc_rs_encoder.cc			\
	atsc_interleaver.cc			\
	atsc_deinterleaver.cc			\
	atsc_trellis_encoder.cc			\
	atsc_viterbi_decoder.cc			\
	atsc_ds_to_softds.cc			\
	atsc_field_sync_mux.cc			\
	atsc_field_sync_demux.cc		\
	atsc_equalizer.cc			\
	atsc_fs_checker.cc			\
	atsc_bit_timing_loop.cc			\
	atsc_fpll.cc				\
	atsc_depad.cc				\
	atsc_pad.cc				\
	atsci_basic_trellis_encoder.cc		\
	atsci_data_interleaver.cc		\
	atsci_equalizer.cc			\
	atsci_equalizer_lms.cc			\
	atsci_equalizer_lms2.cc			\
	atsci_equalizer_nop.cc			\
	atsci_fake_single_viterbi.cc		\
	atsci_fs_checker.cc			\
	atsci_fs_checker_naive.cc		\
	atsci_fs_correlator.cc			\
	atsci_fs_correlator_naive.cc		\
	atsci_single_viterbi.cc			\
	atsci_sssr.cc				\
	atsci_pnXXX.cc				\
	atsci_randomizer.cc			\
	atsci_reed_solomon.cc			\
	atsci_sliding_correlator.cc		\
	atsci_trellis_encoder.cc		\
	atsci_viterbi_decoder.cc		\
	create_atsci_equalizer.cc		\
	create_atsci_fs_checker.cc		\
	create_atsci_fs_correlator.cc		\
	plinfo.cc


BUILT_SOURCES += 				\
	atsci_viterbi_mux.cc

libgnuradio_atsc_la_LIBADD = 			\
	$(GNURADIO_CORE_LA)

libgnuradio_atsc_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)

noinst_LTLIBRARIES = libgnuradio-atsc-qa.la

libgnuradio_atsc_qa_la_SOURCES = 		\
	qa_atsci_basic_trellis_encoder.cc 	\
	qa_atsci_data_interleaver.cc		\
	qa_atsci_equalizer_nop.cc		\
	qa_atsci_fake_single_viterbi.cc		\
	qa_atsci_fs_correlator.cc		\
	qa_atsci_single_viterbi.cc		\
	qa_atsci_randomizer.cc			\
	qa_atsci_reed_solomon.cc		\
	qa_atsci_sliding_correlator.cc		\
	qa_atsci_trellis_encoder.cc		\
	qa_atsci_viterbi_decoder.cc		\
	qa_convolutional_interleaver.cc		\
	qa_atsci.cc				\
	qa_interleaver_fifo.cc			

libgnuradio_atsc_qa_la_LIBADD =			\
	$(GNURADIO_CORE_LA)

libgnuradio_atsc_qa_la_LDFLAGS = \
	$(NO_UNDEFINED)


# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =				\
	atsc_api.h					\
	atsc_consts.h				\
	atsc_derandomizer.h			\
	atsc_randomizer.h			\
	atsc_rs_decoder.h			\
	atsc_rs_encoder.h			\
	atsc_interleaver.h			\
	atsc_deinterleaver.h			\
	atsc_trellis_encoder.h			\
	atsc_viterbi_decoder.h			\
	atsc_ds_to_softds.h			\
	atsc_field_sync_mux.h			\
	atsc_field_sync_demux.h			\
	atsc_equalizer.h			\
	atsc_fs_checker.h			\
	atsc_bit_timing_loop.h			\
	atsc_fpll.h				\
	atsc_depad.h				\
	atsc_pad.h				\
	atsc_types.h				\
	atsci_basic_trellis_encoder.h		\
	atsci_data_interleaver.h		\
	atsci_diag_output.h			\
	atsci_equalizer.h			\
	atsci_equalizer_lms.h			\
	atsci_equalizer_lms2.h			\
	atsci_equalizer_nop.h			\
	atsci_exp2_lp.h				\
	atsci_fake_single_viterbi.h		\
	atsci_fs_checker.h			\
	atsci_fs_checker_naive.h		\
	atsci_fs_correlator.h			\
	atsci_fs_correlator_naive.h		\
	atsci_pnXXX.h				\
	atsci_randomizer.h			\
	atsci_reed_solomon.h			\
	atsci_root_raised_cosine.h		\
	atsci_root_raised_cosine_bandpass.h	\
	atsci_single_viterbi.h			\
	atsci_slicer_agc.h			\
	atsci_sliding_correlator.h		\
	atsci_sssr.h				\
	atsci_syminfo.h				\
	atsci_sync_tag.h			\
	atsci_trellis_encoder.h			\
	atsci_viterbi_decoder.h			\
	atsci_vsbtx_lp.h			\
	convolutional_interleaver.h		\
	create_atsci_equalizer.h		\
	create_atsci_fs_checker.h		\
	create_atsci_fs_correlator.h		\
	fpll_btloop_coupling.h			\
	interleaver_fifo.h

noinst_HEADERS = 				\
	qa_atsci.h				\
	qa_atsci_basic_trellis_encoder.h	\
	qa_atsci_data_interleaver.h		\
	qa_atsci_equalizer_nop.h		\
	qa_atsci_fake_single_viterbi.h		\
	qa_atsci_fs_correlator.h		\
	qa_atsci_randomizer.h			\
	qa_atsci_reed_solomon.h			\
	qa_atsci_single_viterbi.h		\
	qa_atsci_sliding_correlator.h		\
	qa_atsci_trellis_encoder.h		\
	qa_atsci_viterbi_decoder.h		\
	qa_convolutional_interleaver.h		\
	qa_interleaver_fifo.h


# programs we build but don't install
# FIXME add test_atsc
noinst_PROGRAMS = 				\
	test_atsci

atsci_viterbi_gen$(EXEEXT): $(srcdir)/atsci_viterbi_gen.cc
	$(CXX_FOR_BUILD) -O2 $(srcdir)/atsci_viterbi_gen.cc -o atsci_viterbi_gen$(EXEEXT)

atsci_viterbi_mux.cc: atsci_viterbi_gen$(EXEEXT) 
	./atsci_viterbi_gen$(EXEEXT) -o atsci_viterbi_mux.cc

test_atsci_SOURCES = test_atsci.cc
test_atsci_LDADD   =  		\
	libgnuradio-atsc-qa.la	\
	libgnuradio-atsc.la	\
	$(CPPUNIT_LIBS)

# ------------------------------------------------------------------------
#  Cleanup
# ------------------------------------------------------------------------

CLEANFILES += atsci_viterbi_mux.cc atsci_viterbi_gen$(EXEEXT)

# ------------------------------------------------------------------------
#  This is the swig-ish part of the Makefile.
#  It builds the atsc modules which we'll load into python or guile
# ------------------------------------------------------------------------

TOP_SWIG_DOC_IFILES = \
	atsc_swig_doc.i

TOP_SWIG_IFILES = 		\
	$(TOP_SWIG_DOC_IFILES) 	\
	atsc.i

BUILT_SOURCES += \
	$(TOP_SWIG_DOC_IFILES)

EXTRA_DIST += \
	$(TOP_SWIG_DOC_IFILES)

$(TOP_SWIG_DOC_IFILES):
	`echo "" > $@`

# Install so that they end up available as:
#   import gnuradio
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio
atsc_pythondir_category =	\
	gnuradio

# additional libraries for linking with the SWIG-generated library
atsc_la_swig_libadd =		\
	libgnuradio-atsc.la

# additional SWIG files to be installed
atsc_swig_swiginclude_headers =  \
	$(TOP_SWIG_DOC_IFILES)
