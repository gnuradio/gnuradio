#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, blocks

import math, random

def make_random_complex_tuple(L, scale=1):
    result = []
    for x in range(L):
        result.append(scale*complex(2*random.random()-1,
                                    2*random.random()-1))
    return tuple(result)

def make_random_float_tuple(L, scale=1):
    result = []
    for x in range(L):
        result.append(scale*(2*random.random()-1))
    return tuple(result)

class test_moving_average(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_01(self):
        tb = self.tb

        N = 10000
        seed = 0
        data = make_random_float_tuple(N, 1)
        expected_result = N*[0,]

        src = blocks.vector_source_f(data, False)
        op  = blocks.moving_average_ff(100, 0.001)
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()

        dst_data = dst.data()

        # make sure result is close to zero
        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 1)

    def test_02(self):
        tb = self.tb

        N = 10000
        seed = 0
        data = make_random_complex_tuple(N, 1)
        expected_result = N*[0,]

        src = blocks.vector_source_c(data, False)
        op  = blocks.moving_average_cc(100, 0.001)
        dst = blocks.vector_sink_c()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()

        dst_data = dst.data()

        # make sure result is close to zero
        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 1)

if __name__ == '__main__':
    gr_unittest.run(test_moving_average, "test_moving_average.xml")
