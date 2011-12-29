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

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS = 				\
	digital_api.h		\
	digital_impl_mpsk_snr_est.h		\
	digital_binary_slicer_fb.h		\
	digital_clock_recovery_mm_cc.h		\
	digital_clock_recovery_mm_ff.h		\
	digital_constellation.h			\
	digital_constellation_receiver_cb.h 	\
	digital_constellation_decoder_cb.h 	\
	digital_correlate_access_code_bb.h	\
	digital_costas_loop_cc.h		\
	digital_cma_equalizer_cc.h		\
	digital_crc32.h				\
	digital_fll_band_edge_cc.h		\
	digital_lms_dd_equalizer_cc.h		\
	digital_kurtotic_equalizer_cc.h 	\
	digital_metric_type.h			\
	digital_mpsk_receiver_cc.h		\
	digital_mpsk_snr_est_cc.h		\
	digital_ofdm_cyclic_prefixer.h		\
	digital_ofdm_frame_acquisition.h	\
	digital_ofdm_frame_sink.h		\
	digital_ofdm_insert_preamble.h		\
	digital_ofdm_mapper_bcv.h		\
	digital_ofdm_sampler.h			\
	digital_probe_mpsk_snr_est_c.h		\
	digital_gmskmod_bc.h 			\
	digital_cpmmod_bc.h

libgnuradio_digital_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)
