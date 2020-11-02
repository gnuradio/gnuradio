#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import time
from gnuradio import gr, gr_unittest, blocks


class test_sample_and_hold(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        src_data = 10 * [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1]
        ctrl_data = 10 * [1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0]
        expected_result = 10 * (0, 0, 0, 0, 4, 5, 6, 7,
                                8, 9, 9, 9, 9, 9, 9, 9, 9, 9)

        src = blocks.vector_source_f(src_data)
        ctrl = blocks.vector_source_b(ctrl_data)
        op = blocks.sample_and_hold_ff()
        dst = blocks.vector_sink_f()

        self.tb.connect(src, (op, 0))
        self.tb.connect(ctrl, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()

        result = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result, places=6)


if __name__ == '__main__':
    gr_unittest.run(test_sample_and_hold)
