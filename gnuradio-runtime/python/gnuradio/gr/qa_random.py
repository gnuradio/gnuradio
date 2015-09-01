#!/usr/bin/env python
#
# Copyright 2006,2007,2010 Free Software Foundation, Inc.
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
from scipy.stats import norm, laplace

class test_random(gr_unittest.TestCase):

    # Disclaimer
    def test_0(self):
        print 'NOTE: Following tests are not statistically significant! Check out fulltest_random.py for full testing.'
        self.assertEqual(1,1)

    # Check for range [0,1) of uniform distributed random numbers and print minimal and maximal value
    def test_1(self):
        print '# TEST 1'
        print 'Uniform distributed numbers: Range'
        num_tests = 10000
        values = np.zeros(num_tests)
        rndm = gr.random()
        for k in range(num_tests):
            values[k] = rndm.ran1()
        for value in values:
            self.assertLess(value, 1)
            self.assertGreaterEqual(value, 0)
        print 'Uniform random numbers (num/min/max):', num_tests, min(values), max(values)

    # Check uniformly distributed random numbers on uniformity (without assert, only printing)
    def test_2(self):
        print '# TEST 2'
        print 'Uniform random numbers: Distribution'
        num_tests = 10000
        num_bins = 11
        values = np.zeros(num_tests)
        rndm = gr.random()
        for k in range(num_tests):
            values[k] = rndm.ran1()
        bins = np.linspace(0,1,num_bins) # These are the bin edges!
        hist = np.histogram(values,bins)
        print 'Lower edge bin / upper edge bin / count / expected'
        for k in range(len(hist[0])):
                print hist[1][k], hist[1][k+1], hist[0][k], float(num_tests)/(num_bins-1)

    # Check distribution of normally (gaussian, mean=0, variance=1) distributed random numbers (no assert)
    def test_3(self):
        print '# TEST 3'
        print 'Normal random numbers: Distribution'
        num_tests = 10000
        num_bins = 11
        hist_range = [-5,5]
        values = np.zeros(num_tests)
        rndm = gr.random()
        for k in range(num_tests):
            values[k] = rndm.gasdev()
        bins = np.linspace(hist_range[0],hist_range[1],num_bins)
        hist = np.histogram(values,bins)
        print 'Lower edge bin / upper edge bin / count / expected'
        for k in range(len(hist[0])):
            print hist[1][k], hist[1][k+1], hist[0][k], float(norm.cdf(hist[1][k+1])-norm.cdf(hist[1][k]))*num_tests

    # Check distribution of laplacian (mean=0, variance=1) distributed random numbers (no assert)
    def test_4(self):
        print '# TEST 4'
        print 'Laplacian random numbers: Distribution'
        num_tests = 100000
        num_bins = 11
        hist_range = [-5,5]
        values = np.zeros(num_tests)
        rndm = gr.random()
        for k in range(num_tests):
            values[k] = rndm.laplacian()
        bins = np.linspace(hist_range[0],hist_range[1],num_bins)
        hist = np.histogram(values,bins)
        print 'Lower edge bin / upper edge bin / count / expected'
        for k in range(len(hist[0])):
            print hist[1][k], hist[1][k+1], hist[0][k], float(laplace.cdf(hist[1][k+1])-laplace.cdf(hist[1][k]))*num_tests

if __name__ == '__main__':
    gr_unittest.run(test_random, "test_random.xml")
