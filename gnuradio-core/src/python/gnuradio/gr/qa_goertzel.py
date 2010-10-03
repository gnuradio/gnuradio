#!/usr/bin/env python
#
# Copyright 2006,2007,2010 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest
from math import pi, cos

class test_goertzel(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def make_tone_data(self, rate, freq):
        return [cos(2*pi*x*freq/rate) for x in range(rate)]

    def transform(self, src_data, rate, freq):
	src = gr.vector_source_f(src_data, False)
        dft = gr.goertzel_fc(rate, rate, freq)
	dst = gr.vector_sink_c()
	self.tb.connect(src, dft, dst)
	self.tb.run()
	return dst.data()	

    def test_001(self): # Measure single tone magnitude
	rate = 8000
	freq = 100
	bin = freq
	src_data = self.make_tone_data(rate, freq)
	expected_result = 0.5
	actual_result = abs(self.transform(src_data, rate, bin)[0])
	self.assertAlmostEqual(expected_result, actual_result, places=4)

    def test_002(self): # Measure off frequency magnitude
	rate = 8000
	freq = 100
	bin = freq/2
	src_data = self.make_tone_data(rate, freq)
	expected_result = 0.0
	actual_result = abs(self.transform(src_data, rate, bin)[0])
	self.assertAlmostEqual(expected_result, actual_result, places=4)

if __name__ == '__main__':
    gr_unittest.run(test_goertzel, "test_goertzel.xml")
