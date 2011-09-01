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

TESTS =
EXTRA_DIST += run_tests.in

if PYTHON
SUBDIRS = utils
TESTS += run_tests

digitaldir = $(grpythondir)/digital

noinst_PYTHON = \
	qa_digital.py			\
	qa_binary_slicer_fb.py		\
	qa_clock_recovery_mm.py		\
	qa_cma_equalizer.py		\
	qa_cpm.py			\
	qa_constellation.py		\
	qa_constellation_receiver.py	\
	qa_constellation_decoder_cb.py	\
	qa_correlate_access_code.py	\
	qa_costas_loop_cc.py		\
	qa_crc32.py			\
	qa_fll_band_edge.py		\
	qa_lms_equalizer.py		\
	qa_mpsk_receiver.py

digital_PYTHON = \
	__init__.py		\
	bpsk.py			\
	cpm.py			\
	crc.py			\
	generic_mod_demod.py	\
	gmsk.py			\
	modulation_utils.py	\
	modulation_utils2.py	\
	packet_utils.py		\
	pkt.py			\
	psk.py			\
	psk2.py			\
	qam.py			\
	qpsk.py
endif
