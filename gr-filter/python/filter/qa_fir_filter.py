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

class test_filter(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block ()

    def tearDown(self):
        self.tb = None

    def test_fir_filter_fff_001(self):
        decim = 1
        taps = 20*[0.5, 0.5]
        src_data = 40*[1, 2, 3, 4]
        expected_data = fir_filter(src_data, taps, decim)

        src = blocks.vector_source_f(src_data)
        op  = filter.fir_filter_fff(decim, taps)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_data, result_data, 5)

    def test_fir_filter_fff_002(self):
        decim = 4
        taps = 20*[0.5, 0.5]
        src_data = 40*[1, 2, 3, 4]
        expected_data = fir_filter(src_data, taps, decim)

        src = blocks.vector_source_f(src_data)
        op  = filter.fir_filter_fff(decim, taps)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_data, result_data, 5)

    def test_fir_filter_ccf_001(self):
        decim = 1
        taps = 20*[0.5, 0.5]
        src_data = 40*[1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = fir_filter(src_data, taps, decim)

        src = blocks.vector_source_c(src_data)
        op  = filter.fir_filter_ccf(decim, taps)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

    def test_fir_filter_ccf_002(self):
        decim = 4
        taps = 20*[0.5, 0.5]
        src_data = 40*[1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = fir_filter(src_data, taps, decim)

        src = blocks.vector_source_c(src_data)
        op  = filter.fir_filter_ccf(decim, taps)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

    def test_fir_filter_ccc_001(self):
        decim = 1
        taps = 20*[0.5+1j, 0.5+1j]
        src_data = 40*[1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = fir_filter(src_data, taps, decim)

        src = blocks.vector_source_c(src_data)
        op  = filter.fir_filter_ccc(decim, taps)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)


    def test_fir_filter_ccc_002(self):
        decim = 1
        taps = filter.firdes.low_pass(1, 1, 0.1, 0.01)
        src_data = 10*[1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = fir_filter(src_data, taps, decim)

        src = blocks.vector_source_c(src_data)
        op  = filter.fir_filter_ccc(decim, taps)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

    def test_fir_filter_ccc_003(self):
        decim = 4
        taps = 20*[0.5+1j, 0.5+1j]
        src_data = 40*[1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = fir_filter(src_data, taps, decim)

        src = blocks.vector_source_c(src_data)
        op  = filter.fir_filter_ccc(decim, taps)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)


    def test_fir_filter_scc_001(self):
        decim = 1
        taps = 20*[0.5+1j, 0.5+1j]
        src_data = 40*[1, 2, 3, 4]
        expected_data = fir_filter(src_data, taps, decim)

        src = blocks.vector_source_s(src_data)
        op  = filter.fir_filter_scc(decim, taps)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)


    def test_fir_filter_scc_002(self):
        decim = 4
        taps = 20*[0.5+1j, 0.5+1j]
        src_data = 40*[1, 2, 3, 4]
        expected_data = fir_filter(src_data, taps, decim)

        src = blocks.vector_source_s(src_data)
        op  = filter.fir_filter_scc(decim, taps)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

    def test_fir_filter_fsf_001(self):
        decim = 1
        taps = 20*[0.5, 0.5]
        src_data = 40*[1, 2, 3, 4]
        expected_data = fir_filter(src_data, taps, decim)
        expected_data = [int(e) for e in expected_data]

        src = blocks.vector_source_f(src_data)
        op  = filter.fir_filter_fsf(decim, taps)
        dst = blocks.vector_sink_s()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)


    def test_fir_filter_fsf_002(self):
        decim = 4
        taps = 20*[0.5, 0.5]
        src_data = 40*[1, 2, 3, 4]
        expected_data = fir_filter(src_data, taps, decim)
        expected_data = [int(e) for e in expected_data]

        src = blocks.vector_source_f(src_data)
        op  = filter.fir_filter_fsf(decim, taps)
        dst = blocks.vector_sink_s()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_filter, "test_filter.xml")

