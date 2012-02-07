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

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) \
	$(GR_DIGITAL_INCLUDES) $(WITH_INCLUDES)

lib_LTLIBRARIES = libgnuradio-digital.la

libgnuradio_digital_la_SOURCES = 		\
	digital_impl_mpsk_snr_est.cc		\
	digital_binary_slicer_fb.cc		\
	digital_clock_recovery_mm_cc.cc		\
	digital_clock_recovery_mm_ff.cc		\
	digital_constellation.cc		\
	digital_constellation_receiver_cb.cc	\
	digital_constellation_decoder_cb.cc	\
	digital_correlate_access_code_bb.cc	\
	digital_costas_loop_cc.cc		\
	digital_cma_equalizer_cc.cc		\
	digital_crc32.cc			\
	digital_fll_band_edge_cc.cc		\
	digital_lms_dd_equalizer_cc.cc		\
	digital_kurtotic_equalizer_cc.cc	\
	digital_mpsk_receiver_cc.cc		\
	digital_mpsk_snr_est_cc.cc		\
	digital_ofdm_cyclic_prefixer.cc		\
	digital_ofdm_frame_acquisition.cc	\
	digital_ofdm_frame_sink.cc		\
	digital_ofdm_insert_preamble.cc		\
	digital_ofdm_mapper_bcv.cc		\
	digital_ofdm_sampler.cc			\
	digital_probe_mpsk_snr_est_c.cc		\
	digital_gmskmod_bc.cc			\
	digital_cpmmod_bc.cc

libgnuradio_digital_la_LIBADD =	\
	$(GNURADIO_CORE_LA)

libgnuradio_digital_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)
