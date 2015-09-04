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

    # Check reseed method (init with time and seed as fix number)
    def test_2(self):
        num = 5

        rndm0 = gr.random(42); # init with time
        rndm1 = gr.random(42); # init with fix seed
        for k in range(num):
            x = rndm0.ran1();
            y = rndm1.ran1();
            self.assertEqual(x,y)

        x = np.zeros(num)
        y = np.zeros(num)
        rndm0 = gr.random(42); # init with fix seed 1
        for k in range(num):
            x[k] = rndm0.ran1();
        rndm1.reseed(43); # init with fix seed 2
        for k in range(num):
            y[k] = rndm0.ran1();
        for k in range(num):
            self.assertNotEqual(x[k],y[k])

if __name__ == '__main__':
    gr_unittest.run(test_random, "test_random.xml")
