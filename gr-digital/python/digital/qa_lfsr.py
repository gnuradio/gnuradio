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
import numpy as np
from gnuradio import gr, gr_unittest, digital
from gnuradio.digital.utils import lfsr_args

class test_lfsr(gr_unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_lfsr_001(self):
        reglen = 8
        l = digital.lfsr(1, 1, reglen)
        result_data = []
        for i in range(4 * (reglen + 1)):
            result_data.append(l.next_bit())

        expected_result = 4 * ([1, ] + reglen * [0, ])
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 5)

    def test_lfsr_002(self):
        l = digital.lfsr(*lfsr_args(0b1,5,3,0))
        result_data = [l.next_bit() for _ in range(2*(2**5-1))]
        
        expected_result = [1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0,
                           0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0]*2
        self.assertEqual(expected_result, result_data)

        seq1 = [l.next_bit() for _ in range(2**5-1)]
        seq2 = [l.next_bit() for _ in range(2**5-1)]
        self.assertEqual(seq1,seq2)

        res = (np.convolve(seq1,[1,0,1,0,0,1])%2)
        self.assertTrue(sum(res[5:-5])==0,"LRS not generated properly")

if __name__ == '__main__':
    gr_unittest.run(test_lfsr)
