#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks


class test_stretch(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_stretch_01(self):
        tb = self.tb

        data = 10 * [1, ]
        data0 = [x / 20.0 for x in data]
        data1 = [x / 10.0 for x in data]

        expected_result0 = 10 * [0.05, ]
        expected_result1 = 10 * [0.1, ]

        src0 = blocks.vector_source_f(data0, False)
        src1 = blocks.vector_source_f(data1, False)
        inter = blocks.streams_to_vector(gr.sizeof_float, 2)
        op = blocks.stretch_ff(0.1, 2)
        deinter = blocks.vector_to_streams(gr.sizeof_float, 2)
        dst0 = blocks.vector_sink_f()
        dst1 = blocks.vector_sink_f()

        tb.connect(src0, (inter, 0))
        tb.connect(src1, (inter, 1))
        tb.connect(inter, op)
        tb.connect(op, deinter)
        tb.connect((deinter, 0), dst0)
        tb.connect((deinter, 1), dst1)
        tb.run()

        dst0_data = dst0.data()
        dst1_data = dst1.data()

        self.assertFloatTuplesAlmostEqual(expected_result0, dst0_data, 4)
        self.assertFloatTuplesAlmostEqual(expected_result1, dst1_data, 4)


if __name__ == '__main__':
    gr_unittest.run(test_stretch)
