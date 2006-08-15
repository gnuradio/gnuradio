#
# Copyright 2001,2004,2005,2006 Free Software Foundation, Inc.
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

# Install this stuff so that it ends up as the gnuradio.atsc module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

INCLUDES = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(CPPUNIT_INCLUDES)

#LIBS += $(GNURADIO_CORE_LIBS)

EXTRA_DIST =					\
	gen_encoder.py				\
	qa_atsci_trellis_encoder_t1_input.dat	\
	qa_atsci_trellis_encoder_t1_output.dat	\
	qa_atsci_viterbi_decoder_t1_input.dat	\
	qa_atsci_viterbi_decoder_t1_output.dat	


BUILT_SOURCES = 				\
	atsci_viterbi_mux.cc 			\
	$(swig_built_sources)


TESTS =						\
	test_atsci				


# We build two libtool convenience libraries

noinst_LTLIBRARIES = libatsc.la libatsc-qa.la

# FIXME
#	atsci_exp2_lp.cc			\
#	atsci_root_raised_cosine.cc		\
#	atsci_root_raised_cosine_bandpass.cc	\
#	atsci_vsbtx_lp.cc			\
#

libatsc_la_SOURCES = 				\
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

libatsc_qa_la_SOURCES = 			\
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


# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =				\
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
	interleaver_fifo.h			\
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
	atsci_viterbi_gen			\
	test_atsci				


atsci_viterbi_gen_SOURCES = atsci_viterbi_gen.cc

atsci_viterbi_mux.cc: atsci_viterbi_gen$(EXEEXT) 
	$(MAKE) $(AM_MAKEFLAGS) CXX="$(CXX_FOR_BUILD)" atsci_viterbi_gen$(EXEEXT)
	./atsci_viterbi_gen$(EXEEXT) -o atsci_viterbi_mux.cc


test_atsci_SOURCES = test_atsci.cc
test_atsci_LDADD   = libatsc-qa.la libatsc.la $(GNURADIO_CORE_LIBS) $(CPPUNIT_LIBS)


# ------------------------------------------------------------------------
#  This is the swig-ish part of the Makefile.
#  It builds the atsc module which we'll load into python
# ------------------------------------------------------------------------

SWIGCPPPYTHONARGS = -fvirtual -python -modern $(PYTHON_CPPFLAGS) \
	$(STD_DEFINES_AND_INCLUDES)

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

NON_LOCAL_IFILES =			\
	$(GNURADIO_I)

LOCAL_IFILES = 				\
	$(top_srcdir)/gr-atsc/src/lib/atsc.i				

# These files are built by SWIG.  The first is the C++ glue.
# The second is the python wrapper that loads the _atsc shared library
# and knows how to call our extensions.

swig_built_sources = 			\
	atsc.cc				\
	atsc.py				

# This gets atsc.py installed in the right place
ourpython_PYTHON =			\
	atsc.py

ourlib_LTLIBRARIES = _atsc.la

# These are the source files that go into the shared library
_atsc_la_SOURCES = 			\
	atsc.cc			

# magic flags
_atsc_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

# link the library against some comon swig runtime code and the 
# c++ standard library
_atsc_la_LIBADD = 			\
	$(GNURADIO_CORE_LIBS)		\
	$(PYTHON_LDFLAGS)		\
	libatsc.la			\
	-lstdc++			

atsc.cc atsc.py: atsc.i $(ALL_IFILES)
	$(SWIG) $(SWIGCPPPYTHONARGS) -module atsc -o atsc.cc $(LOCAL_IFILES)

# These swig headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)			

# ------------------------------------------------------------------------
#  Cleanup
# ------------------------------------------------------------------------

CLEANFILES =	   atsci_viterbi_mux.cc
MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc

