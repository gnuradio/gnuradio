#!/usr/bin/env python
#
# Copyright 2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr
import numpy as np
from scipy.stats import norm, laplace, rayleigh
from matplotlib import pyplot as plt

# NOTE: scipy and matplotlib are optional packages and not included in the default gnuradio dependencies

#*** SETUP ***#

# Number of realisations per histogram
num_tests = 1000000

# Set number of bins in histograms
uniform_num_bins = 31
gauss_num_bins = 31
rayleigh_num_bins = 31
laplace_num_bins = 31

rndm = gr.random()  # instance of gnuradio random class (gr::random)

print('All histograms contain', num_tests, 'realisations.')

#*** GENERATE DATA ***#

uniform_values = np.zeros(num_tests)
gauss_values = np.zeros(num_tests)
rayleigh_values = np.zeros(num_tests)
laplace_values = np.zeros(num_tests)

for k in range(num_tests):
    uniform_values[k] = rndm.ran1()
    gauss_values[k] = rndm.gasdev()
    rayleigh_values[k] = rndm.rayleigh()
    laplace_values[k] = rndm.laplacian()

#*** HISTOGRAM DATA AND CALCULATE EXPECTED COUNTS ***#

uniform_bins = np.linspace(0, 1, uniform_num_bins)
gauss_bins = np.linspace(-8, 8, gauss_num_bins)
laplace_bins = np.linspace(-8, 8, laplace_num_bins)
rayleigh_bins = np.linspace(0, 10, rayleigh_num_bins)

uniform_hist = np.histogram(uniform_values, uniform_bins)
gauss_hist = np.histogram(gauss_values, gauss_bins)
rayleigh_hist = np.histogram(rayleigh_values, rayleigh_bins)
laplace_hist = np.histogram(laplace_values, laplace_bins)

uniform_expected = np.zeros(uniform_num_bins - 1)
gauss_expected = np.zeros(gauss_num_bins - 1)
rayleigh_expected = np.zeros(rayleigh_num_bins - 1)
laplace_expected = np.zeros(laplace_num_bins - 1)

for k in range(len(uniform_hist[0])):
    uniform_expected[k] = num_tests / float(uniform_num_bins - 1)

for k in range(len(gauss_hist[0])):
    gauss_expected[k] = float(
        norm.cdf(gauss_hist[1][k + 1]) - norm.cdf(gauss_hist[1][k])) * num_tests

for k in range(len(rayleigh_hist[0])):
    rayleigh_expected[k] = float(rayleigh.cdf(
        rayleigh_hist[1][k + 1]) - rayleigh.cdf(rayleigh_hist[1][k])) * num_tests

for k in range(len(laplace_hist[0])):
    laplace_expected[k] = float(laplace.cdf(
        laplace_hist[1][k + 1]) - laplace.cdf(laplace_hist[1][k])) * num_tests

#*** PLOT HISTOGRAMS AND EXPECTATIONS TAKEN FROM SCIPY ***#

uniform_bins_center = uniform_bins[0:-1] + \
    (uniform_bins[1] - uniform_bins[0]) / 2.0
gauss_bins_center = gauss_bins[0:-1] + (gauss_bins[1] - gauss_bins[0]) / 2.0
rayleigh_bins_center = rayleigh_bins[0:-1] + \
    (rayleigh_bins[1] - rayleigh_bins[0]) / 2.0
laplace_bins_center = laplace_bins[0:-1] + \
    (laplace_bins[1] - laplace_bins[0]) / 2.0

plt.figure(1)

plt.subplot(2, 1, 1)
plt.plot(uniform_bins_center,
         uniform_hist[0], 's--', uniform_bins_center, uniform_expected, 'o:')
plt.xlabel('Bins'), plt.ylabel('Count'), plt.title('Uniform: Distribution')
plt.legend(['histogram gr::random', 'calculation scipy'], loc=1)

plt.subplot(2, 1, 2)
plt.plot(uniform_bins_center, uniform_hist[0] / uniform_expected, 'rs--')
plt.xlabel('Bins'), plt.ylabel('Relative deviation'), plt.title(
    'Uniform: Relative deviation to scipy')

plt.figure(2)

plt.subplot(2, 1, 1)
plt.plot(gauss_bins_center, gauss_hist[0], 's--',
         gauss_bins_center, gauss_expected, 'o:')
plt.xlabel('Bins'), plt.ylabel('Count'), plt.title('Gauss: Distribution')
plt.legend(['histogram gr::random', 'calculation scipy'], loc=1)

plt.subplot(2, 1, 2)
plt.plot(gauss_bins_center, gauss_hist[0] / gauss_expected, 'rs--')
plt.xlabel('Bins'), plt.ylabel('Relative deviation'), plt.title(
    'Gauss: Relative deviation to scipy')

plt.figure(3)

plt.subplot(2, 1, 1)
plt.plot(rayleigh_bins_center,
         rayleigh_hist[0], 's--', rayleigh_bins_center, rayleigh_expected, 'o:')
plt.xlabel('Bins'), plt.ylabel('Count'), plt.title('Rayleigh: Distribution')
plt.legend(['histogram gr::random', 'calculation scipy'], loc=1)


plt.subplot(2, 1, 2)
plt.plot(rayleigh_bins_center, rayleigh_hist[0] / rayleigh_expected, 'rs--')
plt.xlabel('Bins'), plt.ylabel('Relative deviation'), plt.title(
    'Rayleigh: Relative deviation to scipy')

plt.figure(4)

plt.subplot(2, 1, 1)
plt.plot(laplace_bins_center,
         laplace_hist[0], 's--', laplace_bins_center, laplace_expected, 'o:')
plt.xlabel('Bins'), plt.ylabel('Count'), plt.title('Laplace: Distribution')
plt.legend(['histogram gr::random', 'calculation scipy'], loc=1)

plt.subplot(2, 1, 2)
plt.plot(laplace_bins_center, laplace_hist[0] / laplace_expected, 'rs--')
plt.xlabel('Bins'), plt.ylabel('Relative deviation'), plt.title(
    'Laplace: Relative deviation to scipy')

plt.show()
