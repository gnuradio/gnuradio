#!/usr/bin/env python
#
# Copyright 2008,2010,2012,2013 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

from gnuradio import gr, gr_unittest, filter, blocks

def fir_filter(x, taps, decim=1):
    y = []
    x2 = (len(taps)-1)*[0,] + x
    for i in range(0, len(x), decim):
        yi = 0
        for j in range(len(taps)):
            yi += taps[len(taps)-1-j] * x2[i+j]
        y.append(yi)
    return y

class test_adaptive_filter(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block ()

    def tearDown(self):
        self.tb = None

    def test_adaptive_fir_filter_ccf_001(self):
        decim = 1
        taps = 20*[0.5, 0.5]
        src_data = 40*[1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = fir_filter(src_data, taps, decim)

        src = blocks.vector_source_c(src_data)
        op  = filter.adaptive_fir_ccf("test", decim, taps)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

    def test_adaptive_fir_filter_ccf_002(self):
        decim = 4
        taps = 20*[0.5, 0.5]
        src_data = 40*[1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = fir_filter(src_data, taps, decim)

        src = blocks.vector_source_c(src_data)
        op  = filter.adaptive_fir_ccf("test", decim, taps)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

    def test_adaptive_fir_filter_ccc_001(self):
        decim = 1
        taps = 20*[0.5+1j, 0.5+1j]
        src_data = 40*[1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = fir_filter(src_data, taps, decim)

        src = blocks.vector_source_c(src_data)
        op  = filter.adaptive_fir_ccc("test", decim, taps)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)


    def test_adaptive_fir_filter_ccc_002(self):
        decim = 4
        taps = 20*[0.5+1j, 0.5+1j]
        src_data = 40*[1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = fir_filter(src_data, taps, decim)

        src = blocks.vector_source_c(src_data)
        op  = filter.adaptive_fir_ccc("test", decim, taps)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_adaptive_filter, "test_adaptive_filter.xml")

