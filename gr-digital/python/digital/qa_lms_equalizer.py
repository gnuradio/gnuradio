#!/usr/bin/env python
#
# Copyright 2006,2007,2010,2011,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, digital, blocks

class test_lms_dd_equalizer(gr_unittest.TestCase):

    def setUp(self):
    	self.tb = gr.top_block()

    def tearDown(self):
    	self.tb = None
    	
    def transform(self, src_data, gain, const):
	SRC = blocks.vector_source_c(src_data, False)
	EQU = digital.lms_dd_equalizer_cc(4, gain, 1, const.base())
	DST = blocks.vector_sink_c()
	self.tb.connect(SRC, EQU, DST)
	self.tb.run()
	return DST.data()

    def test_001_identity(self):
    	# Constant modulus signal so no adjustments
        const = digital.constellation_qpsk()
	src_data = const.points()*1000

        N = 100 # settling time
	expected_data = src_data[N:]
	result = self.transform(src_data, 0.1, const)[N:]

        N = -500
        self.assertComplexTuplesAlmostEqual(expected_data[N:], result[N:], 5)

if __name__ == "__main__":
    gr_unittest.run(test_lms_dd_equalizer, "test_lms_dd_equalizer.xml")
