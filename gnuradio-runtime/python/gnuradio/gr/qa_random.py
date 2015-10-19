#!/usr/bin/env python
#
# Copyright 2006,2007,2010,2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
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
