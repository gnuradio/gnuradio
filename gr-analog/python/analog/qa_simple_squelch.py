#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, analog, blocks


class test_simple_squelch(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_simple_squelch_001(self):
        # Test set/gets

        alpha = 0.0001

        thr1 = 10
        thr2 = 20

        op = analog.simple_squelch_cc(thr1, alpha)

        op.set_threshold(thr2)
        t = op.threshold()
        self.assertEqual(thr2, t)

    def test_simple_squelch_002(self):
        alpha = 0.0001
        thr = -25

        src_data = [float(x) / 10.0 for x in range(1, 40)]
        src = blocks.vector_source_c(src_data)
        op = analog.simple_squelch_cc(thr, alpha)
        dst = blocks.vector_sink_c()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        expected_result = src_data
        expected_result[0:20] = 20 * [0, ]

        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 4)


if __name__ == '__main__':
    gr_unittest.run(test_simple_squelch)
