#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
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
        for i in xrange(4*(reglen+1)):
            result_data.append(l.next_bit())

        expected_result = 4*([1,] + reglen*[0,])
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_lfsr, "test_lfsr.xml")

