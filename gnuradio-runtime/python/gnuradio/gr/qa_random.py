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
from scipy.stats import norm, laplace, rayleigh
#from time import sleep

class test_random(gr_unittest.TestCase):

    num_tests = 10000

    # Disclaimer
    def test_0(self):
        print 'NOTE: Following tests are not statistically significant!'
        print 'Realisations per test:',self.num_tests
        self.assertEqual(1,1)

    # Check for range [0,1) of uniform distributed random numbers and print minimal and maximal value
    def test_1(self):
        print '# TEST 1'
        print 'Uniform distributed numbers: Range'
        values = np.zeros(self.num_tests)
        rndm = gr.random()
        for k in range(self.num_tests):
            values[k] = rndm.ran1()
        for value in values:
            self.assertLess(value, 1)
            self.assertGreaterEqual(value, 0)
        print 'Uniform random numbers (num/min/max):', self.num_tests, min(values), max(values)

    # Check uniformly distributed random numbers on uniformity (without assert, only printing)
    def test_2(self):
        print '# TEST 2'
        print 'Uniform random numbers: Distribution'
        num_bins = 11
        values = np.zeros(self.num_tests)
        rndm = gr.random()
        for k in range(self.num_tests):
            values[k] = rndm.ran1()
        bins = np.linspace(0,1,num_bins) # These are the bin edges!
        hist = np.histogram(values,bins)
        print 'Lower edge bin / upper edge bin / count / expected'
        for k in range(len(hist[0])):
                print hist[1][k], hist[1][k+1], hist[0][k], float(self.num_tests)/(num_bins-1)

    # Check distribution of normally (gaussian, mean=0, variance=1) distributed random numbers (no assert)
    def test_3(self):
        print '# TEST 3'
        print 'Normal random numbers: Distribution'
        num_bins = 11
        hist_range = [-5,5]
        values = np.zeros(self.num_tests)
        rndm = gr.random()
        for k in range(self.num_tests):
            values[k] = rndm.gasdev()
        bins = np.linspace(hist_range[0],hist_range[1],num_bins)
        hist = np.histogram(values,bins)
        print 'Lower edge bin / upper edge bin / count / expected'
        for k in range(len(hist[0])):
            print hist[1][k], hist[1][k+1], hist[0][k], float(norm.cdf(hist[1][k+1])-norm.cdf(hist[1][k]))*self.num_tests

    # Check distribution of laplacian (mean=0, variance=1) distributed random numbers (no assert)
    def test_4(self):
        print '# TEST 4'
        print 'Laplacian random numbers: Distribution'
        num_bins = 11
        hist_range = [-5,5]
        values = np.zeros(self.num_tests)
        rndm = gr.random()
        for k in range(self.num_tests):
            values[k] = rndm.laplacian()
        bins = np.linspace(hist_range[0],hist_range[1],num_bins)
        hist = np.histogram(values,bins)
        print 'Lower edge bin / upper edge bin / count / expected'
        for k in range(len(hist[0])):
            print hist[1][k], hist[1][k+1], hist[0][k], float(laplace.cdf(hist[1][k+1])-laplace.cdf(hist[1][k]))*self.num_tests

    # Check distribution of laplacian (mean=0, variance=1) distributed random numbers (no assert)
    def test_5(self):
        print '# TEST 5'
        print 'Rayleigh random numbers: Distribution'
        num_bins = 11
        hist_range = [0,10]
        values = np.zeros(self.num_tests)
        rndm = gr.random()
        for k in range(self.num_tests):
            values[k] = rndm.rayleigh()
        bins = np.linspace(hist_range[0],hist_range[1],num_bins)
        hist = np.histogram(values,bins)
        print 'Lower edge bin / upper edge bin / count / expected'
        for k in range(len(hist[0])):
            print hist[1][k], hist[1][k+1], hist[0][k], float(rayleigh.cdf(hist[1][k+1])-rayleigh.cdf(hist[1][k]))*self.num_tests

    # Check seeds (init with time and seed as fix number)
    def test_6(self):
        print '# TEST 6'
        num = 5

        print 'Some random numbers in [0,1), should change every run:'
        rndm0 = gr.random(0); # init with time
        # NOTE: the sleep increases the executiont time massively, remove assert for convenience
        #sleep(1)
        #rndm1 = gr.random(0); # init with fix seed
        for k in range(num):
            x = rndm0.ran1();
            print x,
        #    y = rndm1.ran1();
        #    print x, '!=', y
        #    self.assertNotEqual(x,y)
        print ' '

        print 'Some random numbers in [0,1) (seed two instances), should be the same every run:'
        rndm0 = gr.random(42); # init with time
        rndm1 = gr.random(42); # init with fix seed
        for k in range(num):
            x = rndm0.ran1();
            y = rndm1.ran1();
            print x, '=', y
            self.assertEqual(x,y)

        print 'Some random numbers in [0,1) (reseed one instance), should be the same every run:'
        x = np.zeros(num)
        y = np.zeros(num)
        rndm0 = gr.random(42); # init with time
        for k in range(num):
            x[k] = rndm0.ran1();
        rndm1.reseed(43); # init with fix seed
        for k in range(num):
            y[k] = rndm0.ran1();
        for k in range(num):
            print x[k], '!=', y[k]
            self.assertNotEqual(x[k],y[k])

if __name__ == '__main__':
    gr_unittest.run(test_random, "test_random.xml")
