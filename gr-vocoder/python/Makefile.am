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

vocoderdir = $(grpythondir)/vocoder

TESTS = run_tests

noinst_PYTHON = \
	qa_alaw_vocoder.py \
	qa_codec2_vocoder.py \
	qa_cvsd_vocoder.py \
	qa_g721_vocoder.py \
	qa_g723_24_vocoder.py \
	qa_g723_40_vocoder.py \
	qa_gsm_full_rate.py \
	qa_ulaw_vocoder.py

vocoder_PYTHON = \
	__init__.py \
	cvsd.py

EXTRA_DIST += run_tests.in
