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
include $(top_srcdir)/Makefile.swig

TESTS =
EXTRA_DIST += $(nobase_guile_DATA)

AM_CPPFLAGS = \
	$(GR_DIGITAL_INCLUDES) \
	$(STD_DEFINES_AND_INCLUDES) \
	$(PYTHON_CPPFLAGS) \
	$(WITH_INCLUDES)

TOP_SWIG_DOC_IFILES = \
	digital_swig_doc.i

BUILT_SOURCES += \
	$(TOP_SWIG_DOC_IFILES)

EXTRA_DIST += \
	$(TOP_SWIG_DOC_IFILES)

$(TOP_SWIG_DOC_IFILES):
	`echo "" > $@`

if GUILE
nobase_guile_DATA = \
	gnuradio/digital.scm
endif

noinst_GUILE  = digital.test


##############################
# SWIG interface and library
TOP_SWIG_IFILES = \
	$(TOP_SWIG_DOC_IFILES) 	\
	digital_swig.i

# Install so that they end up available as:
#   import gnuradio.digital
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/digital
digital_swig_pythondir_category = \
	gnuradio/digital

# additional libraries for linking with the SWIG-generated library
digital_swig_la_swig_libadd = \
	$(abs_top_builddir)/gr-digital/lib/libgnuradio-digital.la

# additional SWIG files to be installed
digital_swig_swiginclude_headers =		\
	digital_binary_slicer_fb.i		\
	digital_clock_recovery_mm_cc.i		\
	digital_clock_recovery_mm_ff.i		\
	digital_constellation.i			\
	digital_constellation_receiver_cb.i 	\
	digital_constellation_decoder_cb.i	\
	digital_correlate_access_code_bb.i	\
	digital_costas_loop_cc.i		\
	digital_cma_equalizer_cc.i		\
	digital_crc32.i				\
	digital_fll_band_edge_cc.i		\
	digital_lms_dd_equalizer_cc.i		\
	digital_kurtotic_equalizer_cc.i		\
	digital_mpsk_receiver_cc.i		\
	digital_mpsk_snr_est_cc.i		\
	digital_ofdm_cyclic_prefixer.i		\
	digital_ofdm_frame_acquisition.i	\
	digital_ofdm_frame_sink.i		\
	digital_ofdm_insert_preamble.i		\
	digital_ofdm_mapper_bcv.i		\
	digital_ofdm_sampler.i			\
	digital_probe_mpsk_snr_est_c.i		\
	digital_gmskmod_bc.i			\
	digital_cpmmod_bc.i			\
	$(TOP_SWIG_DOC_IFILES)

digital_swig_swig_args = \
	$(GR_DIGITAL_INCLUDES)

if GUILE
TESTS += run_guile_tests
endif