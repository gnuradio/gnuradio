#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import math

from gnuradio import gr, gr_unittest, digital

class test_lfsr(gr_unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_lfsr_001(self):
        reglen = 8
        l = digital.lfsr(1, 1, reglen)

        result_data = []
        for i in range(4*(reglen+1)):
            result_data.append(l.next_bit())

        expected_result = 4*([1,] + reglen*[0,])
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_lfsr, "test_lfsr.xml")

