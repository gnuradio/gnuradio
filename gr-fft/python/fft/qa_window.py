#!/usr/bin/env python3
#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
"""
Unit tests for fft.window
"""

import numpy
from gnuradio import gr_unittest
from gnuradio import fft


class test_window(gr_unittest.TestCase):
    """
    Unit tests for fft.window
    """

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_normwin(self):
        """
        Verify window normalization
        """
        win = fft.window.build(
            fft.window.WIN_BLACKMAN_hARRIS,
            21,
            normalize=True)
        power = numpy.sum([x * x for x in win]) / len(win)
        self.assertAlmostEqual(power, 1.0, places=6)


if __name__ == '__main__':
    gr_unittest.run(test_window)
