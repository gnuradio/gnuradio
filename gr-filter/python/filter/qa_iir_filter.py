#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, filter, blocks

class test_iir_filter(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_iir_direct_001(self):
        src_data = (1, 2, 3, 4, 5, 6, 7, 8)
        fftaps = ()
        fbtaps = ()
        expected_result = (0, 0, 0, 0, 0, 0, 0, 0)
        src = blocks.vector_source_f(src_data)
        op = filter.iir_filter_ffd(fftaps, fbtaps)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)

    def test_iir_direct_002(self):
        src_data = (1, 2, 3, 4, 5, 6, 7, 8)
        fftaps = (2,)
        fbtaps = (0,)
        expected_result = (2, 4, 6, 8, 10, 12, 14, 16)
        src = blocks.vector_source_f(src_data)
        op = filter.iir_filter_ffd(fftaps, fbtaps)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)

    def test_iir_direct_003(self):
        src_data = (1, 2, 3, 4, 5, 6, 7, 8)
        fftaps = (2, 11)
        fbtaps = (0,  0)
        expected_result = (2, 15, 28, 41, 54, 67, 80, 93)
        src = blocks.vector_source_f(src_data)
        op = filter.iir_filter_ffd(fftaps, fbtaps)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)

    def test_iir_direct_004(self):
        src_data = (1, 2, 3, 4, 5, 6, 7, 8)
        fftaps = (2, 11)
        fbtaps = (0,  -1)
        expected_result = (2, 13, 15, 26, 28, 39, 41, 52)
        src = blocks.vector_source_f(src_data)
        op = filter.iir_filter_ffd(fftaps, fbtaps)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)

    def test_iir_direct_005(self):
        src_data = (1, 2, 3, 4, 5, 6, 7, 8)
        fftaps = (2, 11,  0)
        fbtaps = (0,  -1, 3)
        expected_result = (2, 13, 21, 59, 58, 186, 68, 583)
        src = blocks.vector_source_f(src_data)
        op = filter.iir_filter_ffd(fftaps, fbtaps)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)

    def test_iir_direct_006(self):
        src_data = (1, 2, 3, 4, 5, 6, 7, 8)
        expected_result = (2, 13, 21, 59, 58, 186, 68, 583)
        fftaps = (2, 1)
        fbtaps = (0, -1)
        src = blocks.vector_source_f(src_data)
        op = filter.iir_filter_ffd(fftaps, fbtaps)
        fftaps = (2, 11,  0)
        fbtaps = (0,  -1, 3)
        op.set_taps(fftaps, fbtaps)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)

    def test_iir_direct_007(self):
        src_data = (1, 2, 3, 4, 5, 6, 7, 8)
        expected_result = (2,2,5,5,8,8,11,11)
        fftaps = (2, 1)
        fbtaps = (0, -1)
        src = blocks.vector_source_f(src_data)
        op = filter.iir_filter_ffd(fftaps, fbtaps)
        fftaps = (2,0,1)
        fbtaps = (0, -1)
        op.set_taps(fftaps, fbtaps)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)

    def test_iir_direct_008(self):
        src_data = (1, 2, 3, 4, 5, 6, 7, 8)
        expected_result = (2,4,4,10,18,14,26,56)
        fftaps = (2,)
        fbtaps = (0, 1)
        src = blocks.vector_source_f(src_data)
        op = filter.iir_filter_ffd(fftaps, fbtaps)
        fftaps_data = (1)
        fbtaps = (0,0, -1,3)
        op.set_taps(fftaps, fbtaps)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data)

    def test_iir_ccf_001(self):
        src_data = (1+1j, 2+2j, 3+3j, 4+4j, 5+5j, 6+6j, 7+7j, 8+8j)
        expected_result = (2+2j, (6+6j), (12+12j), (20+20j), (30+30j), (42+42j), (56+56j), (72+72j))
        fftaps = (2,)
        fbtaps = (0, 1)
        src = blocks.vector_source_c(src_data)
        op = filter.iir_filter_ccf(fftaps, fbtaps)
        dst = blocks.vector_sink_c()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data)

    def test_iir_ccf_002(self):
        src_data = (1+1j, 2+2j, 3+3j, 4+4j, 5+5j, 6+6j, 7+7j, 8+8j)
        expected_result = (2+2j, (6+6j), (12+12j), (20+20j), (30+30j), (42+42j), (56+56j), (72+72j))

        src = blocks.vector_source_c(src_data)
        op = filter.iir_filter_ccf([1], [1])
        dst = blocks.vector_sink_c()

        fftaps = (2,)
        fbtaps = (0, 1)
        op.set_taps(fftaps, fbtaps)

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data)


    def test_iir_ccd_001(self):
        src_data = (1+1j, 2+2j, 3+3j, 4+4j, 5+5j, 6+6j, 7+7j, 8+8j)
        expected_result = (2+2j, (6+6j), (12+12j), (20+20j), (30+30j), (42+42j), (56+56j), (72+72j))
        fftaps = (2,)
        fbtaps = (0, 1)
        src = blocks.vector_source_c(src_data)
        op = filter.iir_filter_ccd(fftaps, fbtaps)
        dst = blocks.vector_sink_c()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data)

    def test_iir_ccd_002(self):
        src_data = (1+1j, 2+2j, 3+3j, 4+4j, 5+5j, 6+6j, 7+7j, 8+8j)
        expected_result = (2+2j, (6+6j), (12+12j), (20+20j), (30+30j), (42+42j), (56+56j), (72+72j))

        src = blocks.vector_source_c(src_data)
        op = filter.iir_filter_ccd([1], [1])
        dst = blocks.vector_sink_c()

        fftaps = (2,)
        fbtaps = (0, 1)
        op.set_taps(fftaps, fbtaps)

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data)


    def test_iir_ccc_001(self):
        src_data = (1+1j, 2+2j, 3+3j, 4+4j, 5+5j, 6+6j, 7+7j, 8+8j)
        expected_result = (4j, 12j, 24j, 40j, 60j, 84j, 112j, 144j)
        fftaps = (2+2j,)
        fbtaps = (0, 1)
        src = blocks.vector_source_c(src_data)
        op = filter.iir_filter_ccc(fftaps, fbtaps)
        dst = blocks.vector_sink_c()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data)

    def test_iir_ccc_002(self):
        src_data = (1+1j, 2+2j, 3+3j, 4+4j, 5+5j, 6+6j, 7+7j, 8+8j)
        expected_result = (4j, 12j, 24j, 40j, 60j, 84j, 112j, 144j)

        src = blocks.vector_source_c(src_data)
        op = filter.iir_filter_ccc([1], [1])
        dst = blocks.vector_sink_c()

        fftaps = (2+2j,)
        fbtaps = (0, 1)
        op.set_taps(fftaps, fbtaps)

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data)

    def test_iir_ccz_001(self):
        src_data = (1+1j, 2+2j, 3+3j, 4+4j, 5+5j, 6+6j, 7+7j, 8+8j)
        expected_result = (4j, 12j, 24j, 40j, 60j, 84j, 112j, 144j)
        fftaps = (2+2j,)
        fbtaps = (0, 1)
        src = blocks.vector_source_c(src_data)
        op = filter.iir_filter_ccz(fftaps, fbtaps)
        dst = blocks.vector_sink_c()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data)

    def test_iir_ccz_002(self):
        src_data = (1+1j, 2+2j, 3+3j, 4+4j, 5+5j, 6+6j, 7+7j, 8+8j)
        expected_result = (4j, 12j, 24j, 40j, 60j, 84j, 112j, 144j)

        src = blocks.vector_source_c(src_data)
        op = filter.iir_filter_ccz([1], [1])
        dst = blocks.vector_sink_c()

        fftaps = (2+2j,)
        fbtaps = (0, 1)
        op.set_taps(fftaps, fbtaps)

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_iir_filter, "test_iir_filter.xml")

