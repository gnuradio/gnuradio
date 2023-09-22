#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import math
import random
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
        rate = 8000
        freq = 100
        other_freq = 103.5
        level = 0.01
        length = 0
        ramp = 0
        gate = True

        random.seed(1)
        src_data = [0.5 * math.sin(2 * math.pi * 1000 * x / rate) + random.gauss(0, 0.1) for x in range(rate)]

        # First half-second has incorrect CTCSS tone
        for x in range(0, int(rate * 0.500)):
            src_data[x] += 0.15 * math.sin(2 * math.pi * other_freq * x / rate)

        # Second half-second has correct CTCSS tone
        for x in range(int(rate * 0.500), rate):
            src_data[x] += 0.15 * math.sin(2 * math.pi * freq * x / rate)

        src = blocks.vector_source_f(src_data)
        op = analog.ctcss_squelch_ff(rate, freq, level,
                                     length, ramp, gate)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()

        # Squelch should open ~100 ms after the correct CTCSS tone appears
        # so ~400 ms of audio should make it past the gate
        self.assertGreater(len(result_data), rate * 0.390)
        self.assertLess(len(result_data), rate * 0.410)
        self.assertFloatTuplesAlmostEqual(src_data[-len(result_data):], result_data, 6)

    def test_ctcss_squelch_003(self):
        # Test runtime, gate=False
        rate = 8000
        freq = 100
        other_freq = 103.5
        level = 0.01
        length = 0
        ramp = 0
        gate = False

        random.seed(1)
        src_data = [0.5 * math.sin(2 * math.pi * 1000 * x / rate) + random.gauss(0, 0.1) for x in range(rate)]

        # First half-second has incorrect CTCSS tone
        for x in range(0, rate // 2):
            src_data[x] += 0.15 * math.sin(2 * math.pi * other_freq * x / rate)

        # Second half-second has correct CTCSS tone
        for x in range(rate // 2, rate):
            src_data[x] += 0.15 * math.sin(2 * math.pi * freq * x / rate)

        src = blocks.vector_source_f(src_data)
        op = analog.ctcss_squelch_ff(rate, freq, level,
                                     length, ramp, gate)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()

        # Squelch should open ~100 ms after the correct CTCSS tone appears
        min_zero_samples = int(rate * 0.590)
        self.assertFloatTuplesAlmostEqual([0] * min_zero_samples, result_data[:min_zero_samples], 6)
        max_zero_samples = int(rate * 0.610)
        self.assertFloatTuplesAlmostEqual(src_data[max_zero_samples:], result_data[max_zero_samples:], 6)


if __name__ == '__main__':
    gr_unittest.run(test_ctcss_squelch)
