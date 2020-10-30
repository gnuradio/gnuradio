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

import math


class test_transcendental(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_01(self):
        tb = self.tb

        data = 100 * [0, ]
        expected_result = 100 * [1, ]

        src = blocks.vector_source_f(data, False)
        op = blocks.transcendental("cos", "float")
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()

        dst_data = dst.data()

        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 5)

    def test_02(self):
        tb = self.tb

        data = 100 * [3, ]
        expected_result = 100 * [math.log10(3), ]

        src = blocks.vector_source_f(data, False)
        op = blocks.transcendental("log10", "float")
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()

        dst_data = dst.data()

        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 5)

    def test_03(self):
        tb = self.tb

        data = 100 * [3, ]
        expected_result = 100 * [math.tanh(3), ]

        src = blocks.vector_source_f(data, False)
        op = blocks.transcendental("tanh", "float")
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()

        dst_data = dst.data()

        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 5)


if __name__ == '__main__':
    gr_unittest.run(test_transcendental)
