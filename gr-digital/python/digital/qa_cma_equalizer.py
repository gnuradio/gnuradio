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

class test_cma_equalizer_fir(gr_unittest.TestCase):

    def setUp(self):
    	self.tb = gr.top_block()

    def tearDown(self):
    	self.tb = None
    	
    def transform(self, src_data):
	SRC = blocks.vector_source_c(src_data, False)
	EQU = digital.cma_equalizer_cc(4, 1.0, .001, 1)
	DST = blocks.vector_sink_c()
	self.tb.connect(SRC, EQU, DST)
	self.tb.run()
	return DST.data()

    def test_001_identity(self):
    	# Constant modulus signal so no adjustments
	src_data      = (1+0j, 0+1j, -1+0j, 0-1j)*1000
	expected_data = src_data
	result = self.transform(src_data)

        N = -500
        self.assertComplexTuplesAlmostEqual(expected_data[N:], result[N:])

if __name__ == "__main__":
    gr_unittest.run(test_cma_equalizer_fir, "test_cma_equalizer_fir.xml")
