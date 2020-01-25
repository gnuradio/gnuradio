#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import division

from gnuradio import gr, gr_unittest, analog, blocks

class test_ctcss_squelch(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_ctcss_squelch_001(self):
        # Test set/gets

        rate = 1
        rate2 = 2
        freq = 100
        level = 0.5
        length = 1
        ramp = 1
        ramp2 = 2
        gate = True
        gate2 = False

        op = analog.ctcss_squelch_ff(rate, freq, level,
                                     length, ramp, gate)

        op.set_ramp(ramp2)
        r = op.ramp()
        self.assertEqual(ramp2, r)

        op.set_gate(gate2)
        g = op.gate()
        self.assertEqual(gate2, g)

    def test_ctcss_squelch_002(self):
        # Test runtime, gate=True
        rate = 1
        freq = 100
        level = 0.0
        length = 1
        ramp = 1
        gate = True

        src_data = [float(x) / 10.0 for x in range(1, 40)]
        expected_result = src_data
        expected_result[0] = 0

        src = blocks.vector_source_f(src_data)
        op = analog.ctcss_squelch_ff(rate, freq, level,
                                     length, ramp, gate)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 4)

    def test_ctcss_squelch_003(self):
        # Test runtime, gate=False
        rate = 1
        freq = 100
        level = 0.5
        length = 1
        ramp = 1
        gate = False

        src_data = [float(x) / 10.0 for x in range(1, 40)]
        src = blocks.vector_source_f(src_data)
        op = analog.ctcss_squelch_ff(rate, freq, level,
                                     length, ramp, gate)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        expected_result = src_data
        expected_result[0:5] = [0, 0, 0, 0, 0]

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 4)

if __name__ == '__main__':
    gr_unittest.run(test_ctcss_squelch, "test_ctcss_squelch.xml")

