#!/usr/bin/env python
#
# Copyright 2006,2007,2010,2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest
import numpy as np


class test_random(gr_unittest.TestCase):
    # NOTE: For tests on the output distribution of the random numbers, see gnuradio-runtime/apps/evaluation_random_numbers.py.

    # Check for range [0,1) of uniform distributed random numbers
    def test_1(self):
        num_tests = 10000
        values = np.zeros(num_tests)
        rndm = gr.random()
        for k in range(num_tests):
            values[k] = rndm.ran1()
        for value in values:
            self.assertLess(value, 1)
            self.assertGreaterEqual(value, 0)

    # Same seed should yield same random values.
    def test_2_same_seed(self):
        num = 5
        # Init with fixed seed.
        rndm0 = gr.random(42)
        rndm1 = gr.random(42)
        for k in range(num):
            x = rndm0.ran1()
            y = rndm1.ran1()
            self.assertEqual(x, y)

    # reseed should yield same numbers.
    def test_003_reseed(self):
        num = 5
        x = np.zeros(num)
        y = np.zeros(num)
        rndm = gr.random(43)  # init with fix seed 1
        for k in range(num):
            x[k] = rndm.ran1()
        rndm.reseed(43)  # init with fix seed 2
        for k in range(num):
            y[k] = rndm.ran1()
        self.assertFloatTuplesAlmostEqual(x, y)

    def test_004_integer(self):
        nitems = 100000
        minimum = 2
        maximum = 42
        rng = gr.random(1, minimum, maximum)
        rnd_vals = np.zeros(nitems, dtype=int)
        for i in range(nitems):
            rnd_vals[i] = rng.ran_int()
        self.assertGreaterEqual(minimum, np.min(rnd_vals))
        self.assertLess(np.max(rnd_vals), maximum)


if __name__ == '__main__':
    gr_unittest.run(test_random, "test_random.xml")
