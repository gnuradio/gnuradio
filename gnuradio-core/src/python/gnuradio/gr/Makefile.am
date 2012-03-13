#
# Copyright 2004,2005,2006,2008,2010 Free Software Foundation, Inc.
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

EXTRA_DIST += 			\
	run_tests.in		\
	test_16bit_1chunk.wav

TESTS = run_tests


grgrpythondir = $(grpythondir)/gr

grgrpython_PYTHON = 		\
	__init__.py		\
	exceptions.py		\
	gr_threading.py		\
	gr_threading_23.py	\
	gr_threading_24.py	\
	hier_block2.py		\
	prefs.py		\
	top_block.py		\
	pubsub.py

noinst_PYTHON = 			\
	benchmark_filters.py		\
	qa_add_and_friends.py		\
	qa_add_v_and_friends.py		\
	qa_agc.py			\
	qa_argmax.py			\
	qa_bin_statistics.py		\
	qa_classify.py			\
	qa_complex_to_xxx.py		\
	qa_conjugate.py			\
	qa_copy.py			\
	qa_delay.py			\
	qa_dc_blocker.py		\
	qa_diff_encoder.py		\
	qa_diff_phasor_cc.py		\
	qa_ecc_ccsds_27.py		\
	qa_feval.py			\
	qa_fft.py			\
	qa_fft_filter.py		\
	qa_filter_delay_fc.py		\
	qa_float_to_char.py		\
	qa_float_to_int.py		\
	qa_float_to_short.py		\
	qa_fractional_interpolator.py   \
	qa_frequency_modulator.py	\
	qa_fsk_stuff.py			\
	qa_glfsr_source.py		\
	qa_goertzel.py			\
	qa_head.py			\
	qa_hier_block2.py		\
	qa_hilbert.py			\
	qa_iir.py			\
	qa_int_to_float.py		\
	qa_interleave.py		\
	qa_interp_fir_filter.py		\
	qa_kludge_copy.py		\
	qa_kludged_imports.py		\
	qa_max.py			\
	qa_message.py			\
	qa_multiply_conjugate.py	\
	qa_mute.py			\
	qa_nlog10.py			\
	qa_noise.py			\
	qa_packed_to_unpacked.py	\
	qa_pipe_fittings.py		\
	qa_pll_carriertracking.py	\
	qa_pll_freqdet.py		\
	qa_pll_refout.py		\
	qa_pn_correlator_cc.py		\
	qa_probe_signal.py		\
	qa_rational_resampler.py	\
	qa_sig_source.py		\
	qa_single_pole_iir.py		\
	qa_single_pole_iir_cc.py	\
	qa_skiphead.py			\
	qa_unpack_k_bits.py		\
	qa_repeat.py                    \
	qa_scrambler.py			\
	qa_udp_sink_source.py		\
	qa_vector_sink_source.py	
