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


class test_pwr_squelch(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_pwr_squelch_001(self):
        # Test set/gets

        alpha = 0.0001

        thr1 = 10
        thr2 = 20
        ramp = 1
        ramp2 = 2
        gate = True
        gate2 = False

        op = analog.pwr_squelch_cc(thr1, alpha, ramp, gate)

        op.set_threshold(thr2)
        t = op.threshold()
        self.assertEqual(thr2, t)

        op.set_ramp(ramp2)
        r = op.ramp()
        self.assertEqual(ramp2, r)

        op.set_gate(gate2)
        g = op.gate()
        self.assertEqual(gate2, g)

    def test_pwr_squelch_002(self):
        # Test runtime, gate=True
        alpha = 0.0001
        thr = -25

        src_data = [float(x) / 10.0 for x in range(1, 40)]
        src = blocks.vector_source_c(src_data)
        op = analog.pwr_squelch_cc(thr, alpha)
        dst = blocks.vector_sink_c()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        expected_result = src_data
        expected_result[0:20] = 20 * [0, ]

        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 4)

    def test_pwr_squelch_003(self):
        # Test set/gets

        alpha = 0.0001

        thr1 = 10
        thr2 = 20
        ramp = 1
        ramp2 = 2
        gate = True
        gate2 = False

        op = analog.pwr_squelch_ff(thr1, alpha, ramp, gate)

        op.set_threshold(thr2)
        t = op.threshold()
        self.assertEqual(thr2, t)

        op.set_ramp(ramp2)
        r = op.ramp()
        self.assertEqual(ramp2, r)

        op.set_gate(gate2)
        g = op.gate()
        self.assertEqual(gate2, g)

    def test_pwr_squelch_004(self):
        alpha = 0.0001
        thr = -25

        src_data = [float(x) / 10.0 for x in range(1, 40)]
        src = blocks.vector_source_f(src_data)
        op = analog.pwr_squelch_ff(thr, alpha)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        expected_result = src_data
        expected_result[0:20] = 20 * [0, ]

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 4)


if __name__ == '__main__':
    gr_unittest.run(test_pwr_squelch)
