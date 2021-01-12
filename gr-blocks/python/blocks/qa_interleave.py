#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2012-2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks


class test_interleave (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_int_001(self):
        lenx = 64
        src0 = blocks.vector_source_f(list(range(0, lenx, 4)))
        src1 = blocks.vector_source_f(list(range(1, lenx, 4)))
        src2 = blocks.vector_source_f(list(range(2, lenx, 4)))
        src3 = blocks.vector_source_f(list(range(3, lenx, 4)))
        op = blocks.interleave(gr.sizeof_float)
        dst = blocks.vector_sink_f()

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(src2, (op, 2))
        self.tb.connect(src3, (op, 3))
        self.tb.connect(op, dst)
        self.tb.run()
        expected_result = tuple(range(lenx))
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)

    def test_int_002(self):
        blksize = 4
        lenx = 64
        def plusup_big(a): return a + (blksize * 4)
        def plusup_little(a): return a + blksize
        a_vec = list(range(0, blksize))
        for i in range(0, (lenx // (4 * blksize)) - 1):
            a_vec += list(map(plusup_big, a_vec[len(a_vec) - blksize:]))

        b_vec = list(map(plusup_little, a_vec))
        c_vec = list(map(plusup_little, b_vec))
        d_vec = list(map(plusup_little, c_vec))

        src0 = blocks.vector_source_f(a_vec)
        src1 = blocks.vector_source_f(b_vec)
        src2 = blocks.vector_source_f(c_vec)
        src3 = blocks.vector_source_f(d_vec)
        op = blocks.interleave(gr.sizeof_float, blksize)
        dst = blocks.vector_sink_f()

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(src2, (op, 2))
        self.tb.connect(src3, (op, 3))
        self.tb.connect(op, dst)
        self.tb.run()
        expected_result = tuple(range(lenx))
        result_data = dst.data()

        self.assertFloatTuplesAlmostEqual(expected_result, result_data)

    def test_deint_001(self):
        lenx = 64
        src = blocks.vector_source_f(list(range(lenx)))
        op = blocks.deinterleave(gr.sizeof_float)
        dst0 = blocks.vector_sink_f()
        dst1 = blocks.vector_sink_f()
        dst2 = blocks.vector_sink_f()
        dst3 = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect((op, 0), dst0)
        self.tb.connect((op, 1), dst1)
        self.tb.connect((op, 2), dst2)
        self.tb.connect((op, 3), dst3)
        self.tb.run()

        expected_result0 = tuple(range(0, lenx, 4))
        expected_result1 = tuple(range(1, lenx, 4))
        expected_result2 = tuple(range(2, lenx, 4))
        expected_result3 = tuple(range(3, lenx, 4))

        self.assertFloatTuplesAlmostEqual(expected_result0, dst0.data())
        self.assertFloatTuplesAlmostEqual(expected_result1, dst1.data())
        self.assertFloatTuplesAlmostEqual(expected_result2, dst2.data())
        self.assertFloatTuplesAlmostEqual(expected_result3, dst3.data())

    def test_deint_002(self):
        blksize = 4
        lenx = 64
        src = blocks.vector_source_f(list(range(lenx)))
        op = blocks.deinterleave(gr.sizeof_float, blksize)
        dst0 = blocks.vector_sink_f()
        dst1 = blocks.vector_sink_f()
        dst2 = blocks.vector_sink_f()
        dst3 = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect((op, 0), dst0)
        self.tb.connect((op, 1), dst1)
        self.tb.connect((op, 2), dst2)
        self.tb.connect((op, 3), dst3)
        self.tb.run()

        def plusup_big(a): return a + (blksize * 4)
        def plusup_little(a): return a + blksize
        a_vec = list(range(0, blksize))
        for i in range(0, (lenx // (4 * blksize)) - 1):
            a_vec += list(map(plusup_big, a_vec[len(a_vec) - blksize:]))

        b_vec = list(map(plusup_little, a_vec))
        c_vec = list(map(plusup_little, b_vec))
        d_vec = list(map(plusup_little, c_vec))

        expected_result0 = tuple(a_vec)
        expected_result1 = tuple(b_vec)
        expected_result2 = tuple(c_vec)
        expected_result3 = tuple(d_vec)

        self.assertFloatTuplesAlmostEqual(expected_result0, dst0.data())
        self.assertFloatTuplesAlmostEqual(expected_result1, dst1.data())
        self.assertFloatTuplesAlmostEqual(expected_result2, dst2.data())
        self.assertFloatTuplesAlmostEqual(expected_result3, dst3.data())


if __name__ == '__main__':
    gr_unittest.run(test_interleave)
