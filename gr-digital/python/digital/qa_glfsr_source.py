#!/usr/bin/env python
#
# Copyright 2007,2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, digital, blocks


class test_glfsr_source(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000_make_b(self):
        src = digital.glfsr_source_b(16)
        self.assertEqual(src.mask(), 0x8016)
        self.assertEqual(src.period(), 2**16 - 1)

    def test_001_degree_b(self):
        self.assertRaises(RuntimeError,
                          lambda: digital.glfsr_source_b(0))
        self.assertRaises(RuntimeError,
                          lambda: digital.glfsr_source_b(65))

    def test_002_correlation_b(self):
        for degree in range(
                1, 11):                # Higher degrees take too long to correlate
            src = digital.glfsr_source_b(degree, False)
            b2f = digital.chunks_to_symbols_bf((-1.0, 1.0), 1)
            dst = blocks.vector_sink_f()
            del self.tb  # Discard existing top block
            self.tb = gr.top_block()
            self.tb.connect(src, b2f, dst)
            self.tb.run()
            self.tb.disconnect_all()
            actual_result = dst.data()
            R = auto_correlate(actual_result)
            # Auto-correlation peak at origin
            self.assertEqual(R[0], float(len(R)))
            for i in range(len(R) - 1):
                # Auto-correlation minimum everywhere else
                self.assertEqual(R[i + 1], -1.0)

    def test_003_make_f(self):
        src = digital.glfsr_source_f(16)
        self.assertEqual(src.mask(), 0x8016)
        self.assertEqual(src.period(), 2**16 - 1)

    def test_004_degree_f(self):
        self.assertRaises(RuntimeError,
                          lambda: digital.glfsr_source_f(0))
        self.assertRaises(RuntimeError,
                          lambda: digital.glfsr_source_f(65))
        
    def test_005_correlation_f(self):
        for degree in range(
                1, 11):                # Higher degrees take too long to correlate
            src = digital.glfsr_source_f(degree, False)
            dst = blocks.vector_sink_f()
            del self.tb  # Discard existing top block
            self.tb = gr.top_block()
            self.tb.connect(src, dst)
            self.tb.run()

            actual_result = dst.data()
            R = auto_correlate(actual_result)
            # Auto-correlation peak at origin
            self.assertEqual(R[0], float(len(R)))
            for i in range(len(R) - 1):
                # Auto-correlation minimum everywhere else
                self.assertEqual(R[i + 1], -1.0)


def auto_correlate(data):
    l = len(data)
    R = [0, ] * l
    for lag in range(l):
        for i in range(l):
            R[lag] += data[i] * data[i - lag]
    return R


if __name__ == '__main__':
    gr_unittest.run(test_glfsr_source)
