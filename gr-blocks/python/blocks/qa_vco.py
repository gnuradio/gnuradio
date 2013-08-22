#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, blocks
import math

def sig_source_f(samp_rate, freq, amp, N):
    t = map(lambda x: float(x)/samp_rate, xrange(N))
    y = map(lambda x: amp*math.cos(2.*math.pi*freq*x), t)
    return y

def sig_source_c(samp_rate, freq, amp, N):
    t = map(lambda x: float(x)/samp_rate, xrange(N))
    y = map(lambda x: math.cos(2.*math.pi*freq*x) + \
                1j*math.sin(2.*math.pi*freq*x), t)
    return y

class test_vco(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001(self):
        src_data = 200*[0,] + 200*[0.5,] + 200*[1,]
        expected_result = 200*[1,] + \
            sig_source_f(1, 0.125, 1, 200) + \
            sig_source_f(1, 0.25, 1, 200)

        src = blocks.vector_source_f(src_data)
        op = blocks.vco_f(1, math.pi/2.0, 1)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 5)


    def test_002(self):
        src_data = 200*[0,] + 200*[0.5,] + 200*[1,]
        expected_result = 200*[1,] + \
            sig_source_c(1, 0.125, 1, 200) + \
            sig_source_c(1, 0.25, 1, 200)

        src = blocks.vector_source_f(src_data)
        op = blocks.vco_c(1, math.pi/2.0, 1)
        dst = blocks.vector_sink_c()

        self.tb.connect(src, op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 5)


if __name__ == '__main__':
    gr_unittest.run(test_vco, "test_vco.xml")

