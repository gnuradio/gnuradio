#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, analog, blocks

def clip(x, lo, hi):
    if(x < lo):
        return lo
    elif(x > hi):
        return hi
    else:
        return x

class test_rail(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_rail_001(self):
        # Test set/gets

        hi1 = 1
        hi2 = 2
        lo1 = -1
        lo2 = -2

        op = analog.rail_ff(lo1, hi1)

        op.set_hi(hi2)
        h = op.hi()
        self.assertEqual(hi2, h)

        op.set_lo(lo2)
        l = op.lo()
        self.assertEqual(lo2, l)

    def test_rail_002(self):
        lo = -0.75
        hi = 0.90
        src_data = [-2, -1, -0.5, -0.25, 0, 0.25, 0.5, 1, 2]
        expected_result = map(lambda x: clip(x, lo, hi), src_data)

        src = blocks.vector_source_f(src_data)
        op = analog.rail_ff(lo, hi)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 4)

if __name__ == '__main__':
    gr_unittest.run(test_rail, "test_rail.xml")

